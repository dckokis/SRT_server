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
	size_t m_max_packet_size = 65536;
public:
	Listener() noexcept(false);
	int ReceiveData() const;
private:
	void SetListenerSocket();
};