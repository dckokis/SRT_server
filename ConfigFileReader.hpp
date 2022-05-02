#ifndef SRT_LISTENER_APP_CONFIGFILEREADER_HPP
#define SRT_LISTENER_APP_CONFIGFILEREADER_HPP

#include <string>
#include <iostream>
#include <vector>
#include "ConfigReaderExceptions.h"
#include <unistd.h>

enum ERRORS {
	NO_ERROR = 0,
	WRONG_FORMAT = 1
};

struct Address {
	std::string protocol;
	std::string port;
};

class ConfigFileReader {
private:
	Address m_receiverAddr;

	Address m_senderAddr;

	ERRORS parseInput(char *input);

	ERRORS m_error = NO_ERROR;

public:
	explicit ConfigFileReader(const std::string &inputFileName);

	const Address &m_getReceiverAddr() const;

	void m_setReceiverAddr(const Address &receiverAddr);

	const Address &m_getSenderAddr() const;

	void m_setSenderAddr(const Address &senderAddr);

private:
	typedef struct File {
		int descriptor;
		int len;
		char *content;
		char *error_message;
	} File;

	static bool readFile(int file, char *buffer, size_t len) {
		size_t readSize = 1;
		while(readSize > 0) {
			readSize = read(file, buffer, len);
			if(readSize < 0 && errno != EINTR) {
				printf("Failed to read file\n");
				return false;
			}
		}
		return true;
	}
};

#endif //SRT_LISTENER_APP_CONFIGFILEREADER_HPP
