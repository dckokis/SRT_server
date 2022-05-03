#include "ConfigFileReader.hpp"

using namespace std;

const Address &ConfigFileReader::m_getReceiverAddr() const {
	return m_receiverAddr;
}

void ConfigFileReader::m_setReceiverAddr(const Address &receiverAddr) {
	m_receiverAddr = receiverAddr;
}

const Address &ConfigFileReader::m_getSenderAddr() const {
	return m_senderAddr;
}

void ConfigFileReader::m_setSenderAddr(const Address &senderAddr) {
	m_senderAddr = senderAddr;
}

ERRORS ConfigFileReader::parseInput(char *input) {
	string inputString(input);
	vector<string> buffer;
	for(size_t notSpace = 0;;) {
		notSpace = inputString.find_first_not_of(" \t", notSpace);
		if(notSpace == string::npos) {
			break;
		}
		auto isSpace = inputString.find_first_of(" \t", notSpace);
		auto length = (isSpace == string::npos) ? string::npos : isSpace - notSpace;
		std::string address(inputString.substr(notSpace, length));

		auto isColon = address.find_first_of(':', 0);
		if(isColon == string::npos) {
			return WRONG_FORMAT;
		}
		auto protocolLen = (isColon == string::npos) ? string::npos : isColon;
		std::string protocol(address.substr(0, protocolLen));
		auto addrLen = address.length() - protocolLen - 1;
		std::string port(address.substr(isColon + 1, addrLen));
		size_t pos = 0;
		while((pos = port.find("\r\n", pos)) != std::string::npos)
			port.replace(pos, 2, "");
		buffer.push_back(protocol);
		buffer.push_back(port);
		if(isSpace == std::string::npos)
			break;
		notSpace = isSpace;
	}
	m_senderAddr.protocol = buffer[0];
	m_senderAddr.port = buffer[1];
	m_receiverAddr.protocol = buffer[2];
	m_receiverAddr.port = buffer[3];
	return NO_ERROR;
}

bool ConfigFileReader::readFile(int file, char *buffer, size_t len) {
	size_t readSize = 1;
	while(readSize > 0) {
		readSize = read(file, buffer, len);
		if(readSize < 0 && errno != EINTR) {
			m_error = FAILED_TO_READ;
			return false;
		}
	}
	return true;
}

ERRORS ConfigFileReader::m_getError() const {
	return m_error;
}

void ConfigFileReader::readConfig(const string &inputFileName) {
	vector<string> read;
	string line;
	File *file = new File;
	file->descriptor = open(inputFileName.c_str(), O_RDONLY);
	if(file->descriptor == -1) {
		m_error = FAILED_TO_OPEN;
		throw ConfigReaderExceptions("FAILED TO OPEN FILE");
	}
	off_t seekPos = lseek(file->descriptor, 0, SEEK_END);
	if(seekPos == (off_t) -1) {
		file->len = -1;
	}
	file->len = (int) seekPos + 1;
	lseek(file->descriptor, 0, SEEK_SET);
	file->content = new char[file->len];
	if(!readFile(file->descriptor, file->content, file->len)) {
		m_error = FAILED_TO_READ;
		throw ConfigReaderExceptions("FAILED TO READ FILE");
	}
	m_error = parseInput(file->content);
	if(m_error == WRONG_FORMAT) {
		throw ConfigReaderExceptions("WRONG CONFIG FORMAT");
	}
	if(close(file->descriptor) == -1)
		m_error = FAILED_TO_CLOSE;
	delete file->content;
	delete file;
}

