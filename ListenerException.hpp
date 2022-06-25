#pragma once

#include <exception>
#include <string>

class ListenerException final : std::runtime_error {
	std::string m_error;

public:
	explicit ListenerException(const std::string &arg) : runtime_error(arg), m_error(arg) {}

	const char *what() const noexcept override {
		return m_error.c_str();
	}
};
