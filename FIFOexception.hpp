#include <stdexcept>

class FIFOexception final : std::runtime_error {
	std::string m_error;

public:
	explicit FIFOexception(const std::string &arg) : runtime_error(arg), m_error(arg) {}

	const char *what() const noexcept override {
		return m_error.c_str();
	}
};