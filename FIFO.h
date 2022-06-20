#pragma once

#include <thread>
#include <mutex>
#include <iostream>
#include <vector>
#include <atomic>

class FIFO final {
	std::mutex m_FifoMutex;

	std::vector<char> m_pData;

	std::atomic<size_t> m_nReady{};

	std::atomic<size_t> m_nFree{};

	std::atomic<size_t> m_nFreeSize{};

	std::atomic<size_t> m_nReadySize{};

	std::atomic<size_t> m_nFullSize{};

	const size_t m_blockSize;

	const size_t m_maxBlocks;

	size_t m_requestedFreeCount{};

	void *m_givenFreePtr{};

	size_t m_requestedReadyCount{};

	void *m_givenReadyPtr{};

	void m_printBuff() const;

public:

	FIFO(size_t blockSize, size_t maxBlocks);

	void *m_getFree(size_t &count);

	void m_addReady(void *data);

	void *m_getReady(size_t &count);

	void m_addFree(void *data);

	size_t m_getFullSize() const;

	size_t m_getBlockSize() const;

	size_t m_getReadySize() const;

	size_t m_getFreeSize() const;

	void m_printStat();
};

