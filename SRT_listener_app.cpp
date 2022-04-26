#include "SRT_listener_app.h"

using namespace std;

int main() {
	auto a = srt_startup();
	cout << a << endl;
	return 0;
}
