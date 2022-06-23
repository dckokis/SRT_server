#pragma once

#include <exception>
#include <string>

class ListenerException final : std::runtime_error {
public:
	explicit ListenerException(const std::string &arg) : runtime_error(arg){}
};
