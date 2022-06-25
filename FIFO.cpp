//
// Created by dckokis on 25.06.2022.
//

#include "FIFO.hpp"

using namespace std;

Block::Block(const array<char, 7 * 188> &block) {
	m_block = block;
}

const array<char, 7 * 188> &Block::getData() const {
	return m_block;
}

void Block::setData(const array<char, 7 * 188> &mBlock) {
	m_block = mBlock;
}

int Block::getBlockSize() {
	return 7 * 188;
}

Block::Block(const char *block) {
	for (int i = 0; i < m_block.size(); i++)
		if(block[i]) {
			m_block[i] = block[i];
		} else {
			throw BlockException("given block size is less than 7 * 188 bytes");
		}
}

size_t FIFO::m_getIndexWrite() const {
	return m_index_write.load();
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
	m_data.reserve(m_blocks_amount);
	m_index_write = 0;
}
