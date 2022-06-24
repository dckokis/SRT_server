#pragma once

#include <iostream>
#include <srt.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <cassert>
#include "ListenerException.hpp"

class Listener final {
	int m_server_port{};

	int m_fifo_descriptor;

	std::string m_fifo_name = "fifo";

	SRTSOCKET m_listener{};

	size_t m_max_packet_size = 1456*8;

public:
	int m_getServerPort() const;

	void m_setServerPort(int serverPort);

	int m_getFifoDescriptor() const;

	void m_setFifoDescriptor(int fifoDescriptor);

	const std::string &m_getFifoName() const;

	void m_setFifoName(const std::string &fifoName);

	SRTSOCKET m_getListener() const;

	void m_setListener(SRTSOCKET listener);

	size_t m_getMaxPacketSize() const;

	void m_setMaxPacketSize(size_t maxPacketSize);

public:
	explicit Listener(const std::string& port) noexcept(false);

	void ReceiveData() const noexcept(false);

	virtual ~Listener();

private:
	void SetListenerSocket(const std::string& port) noexcept(false);
};