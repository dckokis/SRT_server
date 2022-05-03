#include <fstream>
#include "SRT_listener_app.h"

using namespace std;

int main(int argc, char *argv[]) {
	auto configFileName = "config.txt";
	ConfigFileReader parser;
	try {
		parser.readConfig(configFileName);
		auto a = srt_startup();
		cout << a << endl;
		cout << parser.m_getSenderAddr().port << "-Sender port;" << parser.m_getSenderAddr().protocol
			 << "-Sender protocol" << endl;
		cout << parser.m_getReceiverAddr().port << "-Receiver port;" << parser.m_getReceiverAddr().protocol
			 << "-Receiver protocol" << endl;
		srt_cleanup();
		return 0;
	}
	catch(ConfigReaderExceptions &exception) {
		auto error = parser.m_getError();
		/*
		 * необходимо обработать каждый вариант ошибки по своему
		 * */
		cerr << exception.what() << endl;
		srt_cleanup();
		return 1;
	}
}
