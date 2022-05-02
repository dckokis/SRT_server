#include <fstream>
#include "SRT_listener_app.h"

using namespace std;

int main(int argc, char *argv[]) {
	auto configFileName = "config.txt";
	ConfigFileReader parser(configFileName);
	auto a = srt_startup();
	cout << a << endl;
	return 0;
}
