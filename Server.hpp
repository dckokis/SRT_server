#pragma once

#include <iostream>
#include <srt.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <cassert>
#include "ServerException.hpp"
#include "FIFO.hpp"

class Server {
private:
	int m_server_port{};

	FIFO & m_fifo;

	SRTSOCKET m_server{};

	size_t m_max_packet_size = Block::getBlockSize();

	int m_index_read = 0;
public:
	[[nodiscard]] int m_getServerPort() const;

	[[nodiscard]] SRTSOCKET m_getServer() const;

	[[nodiscard]] size_t m_getMaxPacketSize() const;

public:
	explicit Server(const std::string &port, FIFO &fifo) noexcept(false);

	void SendData() noexcept(false);

	virtual ~Server();

private:
	void SetServerSocket(const std::string &port) noexcept(false);

};

