#pragma once

#include <iostream>
#include <srt.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <cassert>
#include "ListenerException.hpp"
#include "FIFO.hpp"

class Listener final {
	FIFO &m_fifo;

	int m_server_port{};

	SRTSOCKET m_listener{};

	int m_max_packet_size = Block::getBlockSize();
public:
	int m_getServerPort() const;

	SRTSOCKET m_getListener() const;

	size_t m_getMaxPacketSize() const;

public:
	explicit Listener(const std::string &port, FIFO &fifo) noexcept(false);

	void ReceiveData() noexcept(false);

	virtual ~Listener();

private:
	void SetListenerSocket(const std::string &port) noexcept(false);
};