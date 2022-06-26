#pragma once

#include <vector>
#include <array>
#include <mutex>
#include <atomic>
#include "FIFOexception.hpp"

struct Block {
	//// 7 * 188 bytes is a size of 1 TS packet ////
private:
	std::array<char, 7 * 188> m_block = {0};

public:
	explicit Block(const std::array<char, 7 * 188> &block);

	explicit Block(const char *block) noexcept(false);

	const char* getData() const;

	static int getBlockSize();

	void setData(const std::array<char, 7 * 188> &mBlock);
};

class FIFO {
	std::mutex m_FifoMutex;

	std::vector<Block> m_data;

	const size_t m_blocks_amount;

	int m_index_write;

public:
	int m_getIndexWrite() const;

	const std::vector<Block> &m_getData() const;

	size_t m_getBlocksAmount() const;

	explicit FIFO(size_t blocks_amount = 100);

	void addData(const std::vector<Block> &data);

	std::vector<Block> getData(int begin, int end);

	void eraseData(int begin, int end);
};

