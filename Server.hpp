#pragma once

#include <iostream>
#include <srt.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <cassert>
#include "ServerException.hpp"

class Server {
private:
	int m_server_port{};

	int m_fifo_descriptor;

	std::string m_fifo_name = "fifo";

	SRTSOCKET m_server{};

	size_t m_max_packet_size = 1456 * 8;

public:
	explicit Server(const std::string &port) noexcept(false);

	void SendData() const noexcept(false);

	virtual ~Server();

private:
	void SetServerSocket(const std::string &port) noexcept(false);
};

