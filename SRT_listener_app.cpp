#include <fstream>
#include "SRT_listener_app.h"

//using namespace std;
//
//int main(int argc, char *argv[]) {
//	auto configFileName = "config.txt";
//	ConfigFileReader parser;
//	try {
//		parser.readConfig(configFileName);
//		auto a = srt_startup();
//		cout << a << endl;
//		cout << parser.m_getSenderAddr().port << "-Sender port;" << parser.m_getSenderAddr().protocol
//			 << "-Sender protocol" << endl;
//		cout << parser.m_getReceiverAddr().port << "-Receiver port;" << parser.m_getReceiverAddr().protocol
//			 << "-Receiver protocol" << endl;
//		srt_cleanup();
//		return 0;
//	}
//	catch(ConfigReaderExceptions &exception) {
//		auto error = parser.m_getError();
//		/*
//		 * необходимо обработать каждый вариант ошибки по своему
//		 * */
//		cerr << exception.what() << endl;
//		srt_cleanup();
//		return 1;
//	}
//}

#include <iostream>
#include <string>
#include <cstring>
#include <srt.h>
#include <cassert>             // assert

using namespace std;

int main(int argc, char *argv[]) {

	auto configFileName = "config.txt";
	ConfigFileReader parser;
	parser.readConfig(configFileName);
	// use this function to initialize the UDT library
	srt_startup();

	addrinfo hints;
	addrinfo *res;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	string service("9000");
	if(2 == argc)
		service = argv[1];

	if(0 != getaddrinfo(NULL, service.c_str(), &hints, &res)) {
		cout << "illegal port number or port is busy.\n" << endl;
		return 0;
	}

	SRTSOCKET sfd = srt_create_socket();
	if(SRT_INVALID_SOCK == sfd) {
		cout << "srt_socket: " << srt_getlasterror_str() << endl;
		return 0;
	}

	 SRT_TRANSTYPE tt = SRTT_LIVE;
	 if (SRT_ERROR == srt_setsockflag(sfd, SRTO_TRANSTYPE, &tt, sizeof tt))
	 {
	    cout << "srt_setsockopt: " << srt_getlasterror_str() << endl;
	    return 0;
	 }

	if(SRT_ERROR == srt_bind(sfd, res->ai_addr, res->ai_addrlen)) {
		cout << "srt_bind: " << srt_getlasterror_str() << endl;
		return 0;
	}

	freeaddrinfo(res);

	cout << "server is ready at port: " << service << endl;

	if(SRT_ERROR == srt_listen(sfd, 1)) {
		cout << "srt_listen: " << srt_getlasterror_str() << endl;
		return 0;
	}

	int epid = srt_epoll_create();
	if(epid < 0) {
		cout << "srt_epoll_create: " << srt_getlasterror_str() << endl;
		return 0;
	}

	int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
	if(SRT_ERROR == srt_epoll_add_usock(epid, sfd, &events)) {
		cout << "srt_epoll_add_usock: " << srt_getlasterror_str() << endl;
		return 0;
	}

	const int srtrfdslenmax = 1;
	SRTSOCKET srtrfds[srtrfdslenmax];
	char data[1500];

	while(true) {
		string in;
		cin >> in;
		if (in == "exit") {
			break;
		}
		int srtrfdslen = srtrfdslenmax;
		int n = srt_epoll_wait(epid, &srtrfds[0], &srtrfdslen, 0, 0, 100, 0, 0, 0, 0);
		assert(n <= srtrfdslen);
		for(int i = 0; i < n; i++) {
			SRTSOCKET s = srtrfds[i];
			SRT_SOCKSTATUS status = srt_getsockstate(s);
			if((status == SRTS_BROKEN) ||
			   (status == SRTS_NONEXIST) ||
			   (status == SRTS_CLOSED)) {
				cout << "source disconnected. status=" << status << endl;
				srt_close(s);
				continue;
			}
			else {
				while(true) {
					int ret = srt_recv(s, data, 0);
					cout << "OK" << endl;
					if(SRT_ERROR == ret) {
						if(SRT_EASYNCRCV != srt_getlasterror(NULL)) {
							cout << "srt_recvmsg: " << srt_getlasterror_str() << endl;
							return 0;
						}
						break;
					}
				}
			}
		}
	}
	srt_close(sfd);
	srt_epoll_release(epid);

	srt_cleanup();

	return 0;
}

