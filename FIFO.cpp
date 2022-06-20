#include "FIFO.h"

using namespace std;

void FIFO::m_printBuff() const {
	for(auto &c : m_pData) {
		cout << c;
	}
	cout << endl;
}

FIFO::FIFO(size_t blockSize, size_t maxBlocks) : m_blockSize(blockSize), m_maxBlocks(maxBlocks), m_pData(maxBlocks * blockSize) {
	m_nFree = 0;
	m_nReady = 0;
	m_nFreeSize = m_getFullSize();
	m_nReadySize = 0;
	m_nFullSize = blockSize * maxBlocks;
}

void *FIFO::m_getFree(size_t &count) {
	if(m_blockSize == 0 || m_maxBlocks == 0) {
		return nullptr;
	}
	lock_guard<mutex> guard(m_FifoMutex);
	if(count == 0) {
		m_givenFreePtr = nullptr;
		return nullptr;
	} else if(count * m_blockSize > m_nFreeSize) {
		if(m_nFreeSize / m_blockSize == 0) {
			m_givenFreePtr = nullptr;
			return nullptr;
		} else {
			count = m_nFreeSize / m_blockSize;
		}
	}
	m_givenFreePtr = &(m_pData[m_nFree]);
	m_requestedFreeCount = count;
	return m_givenFreePtr;
}

void FIFO::m_addReady(void *data) {
	lock_guard<mutex> guard(m_FifoMutex);
	if(data != m_givenFreePtr || data == nullptr) {
		return;
	}
	size_t count = m_requestedFreeCount;
	if(m_nFree > m_nReady) {
		m_nReadySize += count * m_blockSize;
		m_nFreeSize -= count * m_blockSize;
	} else if(m_nFree < m_nReady) {
		m_nFreeSize -= count * m_blockSize;
	} else if(m_nFree == m_nReady) {
		m_nReadySize += count * m_blockSize;
		m_nFreeSize -= count * m_blockSize;
	}
	m_nFree = (m_nFree + count * m_blockSize);
	if(m_nFree == m_nFullSize) {
		m_nFree = 0;
		m_nFreeSize = m_nReady.load();
	}
}

void *FIFO::m_getReady(size_t &count) {
	if(m_blockSize == 0 || m_maxBlocks == 0) {
		return nullptr;
	}
	lock_guard<mutex> guard(m_FifoMutex);
	if(count == 0) {
		m_givenReadyPtr = nullptr;
		return nullptr;
	}
	if(count * m_blockSize > m_nReadySize) {
		if(m_nReadySize == 0) {
			m_givenReadyPtr = nullptr;
			return nullptr;
		}
		count = m_nReadySize / m_blockSize;
	}
	m_givenReadyPtr = &m_pData[m_nReady];
	m_requestedReadyCount = count;
	return m_givenReadyPtr;
}

void FIFO::m_addFree(void *data) {
	lock_guard<mutex> guard(m_FifoMutex);
	if(data != m_givenReadyPtr || data == nullptr) {
		return;
	}
	size_t count = m_requestedReadyCount;
	if(m_nReady > m_nFree) {
		m_nFreeSize += count * m_blockSize;
		m_nReadySize -= count * m_blockSize;
	} else if(m_nReady < m_nFree) {
		m_nReadySize -= count * m_blockSize;
	} else if(m_nFree == m_nReady) {
		m_nReadySize -= count * m_blockSize;
		m_nFreeSize += count * m_blockSize;
	}
	m_nReady = (m_nReady + count * m_blockSize);
	if(m_nReady == m_nFullSize) {
		m_nReady = 0;
		m_nReadySize = m_nFree.load();
	}
}

size_t FIFO::m_getFullSize() const {
	return m_maxBlocks * m_blockSize;
}

size_t FIFO::m_getBlockSize() const {
	return m_blockSize;
}

size_t FIFO::m_getReadySize() const {
	return m_nReadySize.load();
}

size_t FIFO::m_getFreeSize() const {
	return m_nFreeSize.load();
}

void FIFO::m_printStat() {
	lock_guard<mutex> guard(m_FifoMutex);
	using namespace this_thread;
	cout << endl;
	cout << get_id() << " " << endl;
	cout << get_id() << " " << "free space: " << m_nFreeSize << endl;
	cout << get_id() << " " << "ready space: " << m_nReadySize << endl;
	cout << get_id() << " " << "nfree space: " << m_nFree << endl;
	cout << get_id() << " " << "nready space: " << m_nReady << endl;
	cout << get_id() << " " << "buffer is: ";
	m_printBuff();
	cout << endl;
}
