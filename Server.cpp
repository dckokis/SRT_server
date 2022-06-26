#include "Server.hpp"

using namespace std;

Server::Server(const string &port, FIFO &fifo) : m_fifo(fifo) {
	if(srt_startup() < 0) {
		throw ServerException("failed to startup srt lib");
	}
	try {
		SetServerSocket(port);
	}
	catch(ServerException &ex) {
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
				bool sending = true;
				while(sending) {
					auto tmp_index_reader = m_fifo.m_getIndexWrite();
					if(0 == tmp_index_reader) {
						sending = false;
					}
					auto tmp_data = m_fifo.getData(0, tmp_index_reader);
					m_fifo.eraseData(0, tmp_index_reader);
					for(auto block : tmp_data) { // пытаемся отправить все что было нового в фифо по очереди
						int snd = srt_send(s, block.getData(), block.getPayloadSize());
						if(SRT_ERROR == snd) {
							if(SRT_EASYNCSND != srt_getlasterror(nullptr)) {
								sending = false;
								break;
							} else {
								srt_epoll_remove_usock(i, s);
								srt_close(s);
								sending = false;
								break;
							}
						}
						cout << "message sent" << endl;
					}
				}
			}
		}
		srt_epoll_release(loc_epollId);
	}
}

Server::~Server() {
	srt_close(m_server);
}

int Server::m_getServerPort() const {
	return m_server_port;
}

SRTSOCKET Server::m_getServer() const {
	return m_server;
}

size_t Server::m_getMaxPacketSize() const {
	return m_max_packet_size;
}