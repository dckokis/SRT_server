#include "Listener.hpp"

using namespace std;

Listener::Listener() {
	if(mkfifo(m_fifo_name.c_str(), 0777) != 0) {
		throw ListenerException("failed to make fifo");
	}
	if((m_fifo_descriptor = open(m_fifo_name.c_str(), O_WRONLY)) <= 0) {
		throw ListenerException("failed to open fifo");
	}
	if(srt_startup() < 0) {
		throw ListenerException("failed to startup srt lib");
	}
	try {
		SetListenerSocket();
	}
	catch(ListenerException &ex) {
		close(m_fifo_descriptor);
		throw ex;
	}
}

void Listener::SetListenerSocket() {
	addrinfo loc_addrinfo{};
	addrinfo *loc_res;

	memset(&loc_addrinfo, 0, sizeof(struct addrinfo));
	loc_addrinfo.ai_flags = AI_PASSIVE;
	loc_addrinfo.ai_family = AF_INET;
	loc_addrinfo.ai_socktype = SOCK_DGRAM;

	string loc_service("9000");

	if(0 != getaddrinfo(nullptr, loc_service.c_str(), &loc_addrinfo, &loc_res)) {
		freeaddrinfo(loc_res);
		throw ListenerException(srt_getlasterror_str());
	}

	m_listener = srt_create_socket();
	if(SRT_INVALID_SOCK == m_listener) {
		freeaddrinfo(loc_res);
		throw ListenerException(srt_getlasterror_str());
	}

	auto loc_transtype = SRTT_LIVE;
	if(SRT_ERROR == srt_setsockflag(m_listener, SRTO_TRANSTYPE, &loc_transtype, sizeof loc_transtype)) {
		freeaddrinfo(loc_res);
		throw ListenerException(srt_getlasterror_str());
	}

	if(SRT_ERROR == srt_bind(m_listener, loc_res->ai_addr, loc_res->ai_addrlen)) {
		freeaddrinfo(loc_res);
		throw ListenerException(srt_getlasterror_str());
	}

	freeaddrinfo(loc_res);
	if(SRT_ERROR == srt_listen(m_listener, 1)) {
		throw ListenerException(srt_getlasterror_str());
	}
	m_server_port = stoi(loc_service);
	cout << "server is ready at port: " << m_server_port << endl;
}

int Listener::ReceiveData() const {
	int loc_epollId = srt_epoll_create();
	if(loc_epollId < 0) {
		throw ListenerException(srt_getlasterror_str());
	}

	int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
	if(SRT_ERROR == srt_epoll_add_usock(loc_epollId, m_listener, &events)) {
		throw ListenerException(srt_getlasterror_str());
	}

	const int loc_rfdsMaxLen = 100;
	SRTSOCKET loc_rfds[loc_rfdsMaxLen];
	char data[m_max_packet_size];

	while(true) {
		int rfdsLen = loc_rfdsMaxLen;
		int n = srt_epoll_wait(loc_epollId, &loc_rfds[0], &rfdsLen, nullptr, nullptr, 100, nullptr, nullptr, nullptr, nullptr);
		assert(n <= rfdsLen);
		for(int i = 0; i < n; i++) {
			SRTSOCKET s = loc_rfds[i];
			SRT_SOCKSTATUS status = srt_getsockstate(s);
			if((status == SRTS_BROKEN) ||
			   (status == SRTS_NONEXIST) ||
			   (status == SRTS_CLOSED)) {
				cout << "source disconnected. status=" << status << endl;
				srt_close(s);
				continue;
			} else if(s == m_listener) {
				assert(status == SRTS_LISTENING);

				SRTSOCKET loc_srtsocket;
				sockaddr_storage loc_clientAddress{};
				int loc_addrlen = sizeof(loc_clientAddress);

				loc_srtsocket = srt_accept(m_listener, (sockaddr *) &loc_clientAddress, &loc_addrlen);
				if(SRT_INVALID_SOCK == loc_srtsocket) {
					throw ListenerException(srt_getlasterror_str());
				}

				char loc_clientHost[NI_MAXHOST];
				char loc_clientService[NI_MAXSERV];
				getnameinfo((sockaddr *) &loc_clientAddress, loc_addrlen,
							loc_clientHost, sizeof(loc_clientHost),
							loc_clientService, sizeof(loc_clientService), NI_NUMERICHOST | NI_NUMERICSERV);
				cout << "new connection: " << loc_clientHost << ":" << loc_clientService << endl;

				int loc_events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
				if(SRT_ERROR == srt_epoll_add_usock(loc_epollId, loc_srtsocket, &loc_events)) {
					throw ListenerException(srt_getlasterror_str());
				}
			} else {
				while(true) {
					int ret = srt_recv(s, data, 0);
					////////////////////////////////// fifo write

					if(SRT_ERROR == ret) {
						if(SRT_EASYNCRCV != srt_getlasterror(nullptr)) {
							throw ListenerException(srt_getlasterror_str());
						}
						break;
					}
					cout << "message received" << endl;
				}
			}
		}
	}
	srt_epoll_release(loc_epollId);
	return 0;
}
