#pragma once

#include <exception>
#include <string>
#include <stdexcept>

class ServerException final : std::runtime_error {
	std::string m_error;
public:
	explicit ServerException(const std::string &arg) : runtime_error(arg),  m_error(arg){}
	const char * what() const noexcept override {
		return m_error.c_str();
	}
};

