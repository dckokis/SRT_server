#include <fstream>
#include <thread>

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

#include "Listener.hpp"
#include "Server.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	try {
		FIFO fifo;
		string listener_port("9000");
		string server_port("8000");
		Listener listener(listener_port, fifo);
		Server server(server_port, fifo);
		auto receive = [&listener] {
			return listener.ReceiveData();
		};
		auto send = [&server] {
			return server.SendData();
		};
		std::thread thread_receiver(receive);
		std::thread thread_sender(send);
		thread_receiver.join();
		thread_sender.join();
	}
	catch(ListenerException &ex) {
		cerr << "Listener process failure with a specific exception: " << ex.what() << endl;
	}
	catch(ServerException &ex) {
		cerr << "Server process failure with a specific exception: " << ex.what() << endl;
	}
	catch(exception &ex) {
		cerr << ex.what() << endl;
	}
}


