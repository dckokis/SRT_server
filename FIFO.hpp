#pragma once

#include <vector>
#include <array>
#include <mutex>
#include <atomic>
#include "FIFOexception.hpp"

//короче, в fifo делаем как:
//данным erase не надо, когда до конца дошли - просто затираем данные в начале
//если index write и read отличаются больше чем на 20% от размера - делаем сброс, read = write
//фифошке даю снаружи index read по сслыке, она его меняет и возвращает тру, или не меняет и говорит false,
//тогда забиваю и иду дальше
// вычитывать по одному блоку! соответственно давать один индекс от сервера
struct Block {
	//// 7 * 188 bytes is a size of 1 TS packet ////
private:
	std::array<char, 7 * 188> m_block = {0};

	size_t m_payload_size;

public:
	explicit Block(const char *block, size_t payload_size = 7 * 188) noexcept(false);

	[[nodiscard]] const char *getData() const;

	static int getBlockSize();

	[[nodiscard]] size_t getPayloadSize() const;

};

class FIFO {
	std::mutex m_FifoMutex;

	std::vector<Block> m_data;

	const size_t m_blocks_amount;

	int m_index_write;

	[[nodiscard]] bool checkDifference(int index) const;

	double m_max_delay;

public:
	[[nodiscard]] int m_getIndexWrite() const;

	[[nodiscard]] const std::vector<Block> &m_getData() const;

	[[nodiscard]] size_t m_getBlocksAmount() const;

	explicit FIFO(size_t blocks_amount = 100, double max_delay = 0.2);

	void addData(const Block &data);

	std::pair<bool, int> getData(int& index, const char *data);
};

