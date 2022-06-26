#include <fstream>
#include <thread>
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


