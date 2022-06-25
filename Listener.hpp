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

	void m_setServerPort(int serverPort);


	SRTSOCKET m_getListener() const;

	void m_setListener(SRTSOCKET listener);

	size_t m_getMaxPacketSize() const;

	void m_setMaxPacketSize(int maxPacketSize);

public:
	explicit Listener(const std::string &port, FIFO &fifo) noexcept(false);

	void ReceiveData() const noexcept(false);

	virtual ~Listener();

private:
	void SetListenerSocket(const std::string &port) noexcept(false);
};