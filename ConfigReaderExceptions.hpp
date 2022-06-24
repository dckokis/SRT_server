#ifndef SRT_LISTENER_APP_CONFIGREADEREXCEPTIONS_HPP
#define SRT_LISTENER_APP_CONFIGREADEREXCEPTIONS_HPP

#include <exception>
#include <string>

class ConfigReaderExceptions final : std::exception {
private:
	std::string m_error;

public:
	explicit ConfigReaderExceptions(std::string error) : m_error(std::move(error)) {}

	const char *what() const noexcept override {
		return m_error.c_str();
	}
};

#endif //SRT_LISTENER_APP_CONFIGREADEREXCEPTIONS_HPP
