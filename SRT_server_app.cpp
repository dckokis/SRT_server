#include "Server.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	try {
		string port("8000");
		Server server(port);
	}
	catch(ServerException &ex) {
		cerr << "Server process failure with a specific exception: " << ex.what() << endl;
	}
	catch(exception &ex) {
		cerr << "Server process failure: " << ex.what() << endl;
	}
}

