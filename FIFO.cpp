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

void FIFO::addData(const Block &data) {
	m_FifoMutex.lock();
	if(m_index_write == m_blocks_amount - 1) {
		m_index_write = 0;
	}
	m_data[m_index_write] = data;
	m_index_write++;
	m_FifoMutex.unlock();
}

FIFO::FIFO(size_t blocks_amount, double max_delay) : m_blocks_amount(blocks_amount), m_max_delay(max_delay) {
	m_index_write = 0;
}

pair<bool, int> FIFO::getData(int &index, const char *data) {
	m_FifoMutex.lock();
	if(!checkDifference(index)) {
		m_FifoMutex.unlock();
		index = m_index_write;
		return make_pair(false, 0);
	} else {
		data = m_data[index].getData();
		auto payload = m_data[index].getPayloadSize();
		index++;
		m_FifoMutex.unlock();
		return make_pair(true, payload);
	}
}

bool FIFO::checkDifference(int index) const {
	auto difference = m_index_write > index ? m_index_write - index : m_blocks_amount - 1 - index + m_index_write;
	return abs(difference / m_blocks_amount - m_max_delay) <= 0.0001;
}

