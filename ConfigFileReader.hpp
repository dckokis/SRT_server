#ifndef SRT_LISTENER_APP_CONFIGFILEREADER_HPP
#define SRT_LISTENER_APP_CONFIGFILEREADER_HPP

#include <string>
#include <iostream>
#include <vector>
#include "ConfigReaderExceptions.h"
#include <unistd.h>
#include <fcntl.h>

enum ERRORS {
	NO_ERROR = 0,
	WRONG_FORMAT = 1,
	FAILED_TO_READ = 2,
	FAILED_TO_CLOSE = 3,
	FAILED_TO_OPEN = 4,
};

struct Address {
	std::string protocol;
	std::string port;
};

class ConfigFileReader {
private:
	Address m_receiverAddr;

private:

	Address m_senderAddr;

	ERRORS parseInput(char *input);

	ERRORS m_error = NO_ERROR;

public:
	ConfigFileReader() = default;

	void readConfig(const std::string &inputFileName);

	const Address &m_getReceiverAddr() const;

	void m_setReceiverAddr(const Address &receiverAddr);

	const Address &m_getSenderAddr() const;

	void m_setSenderAddr(const Address &senderAddr);

	ERRORS m_getError() const;

private:
	typedef struct File {
		int descriptor;
		int len;
		char *content;
	} File;

	bool readFile(int file, char *buffer, size_t len);
};

#endif //SRT_LISTENER_APP_CONFIGFILEREADER_HPP
