#include "Server.hpp"

using namespace std;

Server::Server(const string &port) {
	if(mkfifo(m_fifo_name.c_str(), 0777) != 0) {
		throw ServerException("failed to make fifo");
	}
	if((m_fifo_descriptor = open(m_fifo_name.c_str(), O_RDONLY)) <= 0) {
		throw ServerException("failed to open fifo");
	}
	try {
		SetServerSocket(port);
	}
	catch(ServerException &ex) {
		close(m_fifo_descriptor);
		throw;
	}
}

void Server::SetServerSocket(const string &port) noexcept(false) {
	addrinfo loc_addrinfo{};
	addrinfo *loc_res;

	memset(&loc_addrinfo, 0, sizeof(struct addrinfo));
	loc_addrinfo.ai_flags = AI_PASSIVE;
	loc_addrinfo.ai_family = AF_INET;
	loc_addrinfo.ai_socktype = SOCK_DGRAM;

	const string &loc_service(port);

	if(0 != getaddrinfo(nullptr, loc_service.c_str(), &loc_addrinfo, &loc_res)) {
		freeaddrinfo(loc_res);
		throw ServerException(srt_getlasterror_str());
	}

	m_server = srt_create_socket();
	if(SRT_INVALID_SOCK == m_server) {
		freeaddrinfo(loc_res);
		throw ServerException(srt_getlasterror_str());
	}

	auto loc_transtype = SRTT_LIVE;
	bool no = false;
	if(SRT_ERROR == srt_setsockflag(m_server, SRTO_TRANSTYPE, &loc_transtype, sizeof loc_transtype)
	   || SRT_ERROR == srt_setsockopt(m_server, 0, SRTO_RCVSYN, &no, sizeof no)) {
		freeaddrinfo(loc_res);
		throw ServerException(srt_getlasterror_str());
	}

	if(SRT_ERROR == srt_bind(m_server, loc_res->ai_addr, loc_res->ai_addrlen)) {
		freeaddrinfo(loc_res);
		throw ServerException(srt_getlasterror_str());
	}

	freeaddrinfo(loc_res);
	if(SRT_ERROR == srt_listen(m_server, 1)) {
		throw ServerException(srt_getlasterror_str());
	}
	m_server_port = stoi(loc_service);
	cout << "server is ready at port: " << m_server_port << endl;
}

void Server::SendData() const noexcept(false) {
	int loc_epollId = srt_epoll_create();
	if(loc_epollId < 0) {
		throw ServerException(srt_getlasterror_str());
	}

	int events = SRT_EPOLL_OUT | SRT_EPOLL_ERR;
	if(SRT_ERROR == srt_epoll_add_usock(loc_epollId, m_server, &events)) {
		throw ServerException(srt_getlasterror_str());
	}

	const int loc_wfdsMaxLen = 100;
	SRTSOCKET loc_wfds[loc_wfdsMaxLen];

	while(true) { //////////////////// Как тормозить? По таймеру(больше n секунд не было сообщений)?
		int rfdsLen = loc_wfdsMaxLen;
		int n = srt_epoll_wait(loc_epollId, &loc_wfds[0], &rfdsLen, nullptr, nullptr, 100, nullptr, nullptr, nullptr, nullptr);
		assert(n <= rfdsLen);
		for(int i = 0; i < n; i++) {
			SRTSOCKET s = loc_wfds[i];
			SRT_SOCKSTATUS status = srt_getsockstate(s);
			if((status == SRTS_BROKEN) ||
			   (status == SRTS_NONEXIST) ||
			   (status == SRTS_CLOSED)) {
				cout << "source disconnected. status=" << status << endl;
				srt_close(s);
				continue;
			} else if(s == m_server) {
				assert(status == SRTS_LISTENING);

				SRTSOCKET loc_srtsocket;
				sockaddr_storage loc_clientAddress{};
				int loc_addrlen = sizeof(loc_clientAddress);

				loc_srtsocket = srt_accept(m_server, (sockaddr * ) & loc_clientAddress, &loc_addrlen);
				if(SRT_INVALID_SOCK == loc_srtsocket) {
					throw ServerException(srt_getlasterror_str());
				}

				char loc_clientHost[NI_MAXHOST];
				char loc_clientService[NI_MAXSERV];
				getnameinfo((sockaddr * ) & loc_clientAddress, loc_addrlen,
							loc_clientHost, sizeof(loc_clientHost),
							loc_clientService, sizeof(loc_clientService), NI_NUMERICHOST | NI_NUMERICSERV);
				cout << "new connection: " << loc_clientHost << ":" << loc_clientService << endl;

				int loc_events = SRT_EPOLL_OUT | SRT_EPOLL_ERR;
				if(SRT_ERROR == srt_epoll_add_usock(loc_epollId, loc_srtsocket, &loc_events)) {
					throw ServerException(srt_getlasterror_str());
				}
			} else {
				while(true) {
					char data[m_max_packet_size];
					int len = read(m_fifo_descriptor, data, m_max_packet_size);
					int snd = srt_recv(s, data, len);
					if(SRT_ERROR == snd) {
						if(SRT_EASYNCSND != srt_getlasterror(nullptr)) {
							break;/////////////////////// Тут выходим, или ждем и продолжаем?
						} else {
							throw ServerException(srt_getlasterror_str());
						}
					}

					cout << "message sent" << endl;
				}
			}
		}
		srt_epoll_release(loc_epollId);
	}
}
