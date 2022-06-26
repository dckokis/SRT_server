#include "FIFO.hpp"

using namespace std;

const char *Block::getData() const {
	return m_block.data();
}

int Block::getBlockSize() {
	return 7 * 188;
}

Block::Block(const char *block, size_t payload_size) : m_payload_size(payload_size) {
	for(int i = 0; i < payload_size; i++) {
		m_block[i] = block[i];
	}
}

size_t Block::getPayloadSize() const {
	return m_payload_size;
}

int FIFO::m_getIndexWrite() const {
	return m_index_write;
}

const vector<Block> &FIFO::m_getData() const {
	return m_data;
}

size_t FIFO::m_getBlocksAmount() const {
	return m_blocks_amount;
}

void FIFO::addData(const vector<Block> &data) {
	m_FifoMutex.lock();
	if((m_blocks_amount - m_data.size()) < data.size()) {
		m_FifoMutex.unlock();
		throw FIFOexception("size of passed data is bigger than space left in fifo");
	}
	for(auto block : data) {
		m_data.push_back(block);
		m_index_write++;
	}
	m_FifoMutex.unlock();
}

FIFO::FIFO(size_t blocks_amount) : m_blocks_amount(blocks_amount) {
	m_index_write = 0;
}

std::vector<Block> FIFO::getData(int begin, int end) {
	m_FifoMutex.lock();
	auto loc_end = end < m_index_write ? end : m_index_write;
	std::vector<Block> data;
	for(int i = begin; i < loc_end; i++) {
		data.push_back(m_data[i]);
	}
	m_FifoMutex.unlock();
	return data;
}

void FIFO::eraseData(int begin, int end) {
	m_FifoMutex.lock();
	auto loc_end = end < m_index_write ? end : m_index_write;
	m_data.erase(m_data.begin() + begin, m_data.begin() + loc_end);
	m_index_write = begin;
	m_FifoMutex.unlock();
}
