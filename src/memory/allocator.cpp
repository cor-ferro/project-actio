#include "allocator.h"

namespace memory {
	Allocator::Allocator(const std::size_t totalSize){
		m_totalSize = totalSize;
		m_used = 0;
		m_peak = 0;
	}

	Allocator::~Allocator(){
		m_totalSize = 0;
	}

	std::size_t Allocator::getTotatlSize()
	{
		return m_totalSize;
	}

	std::size_t Allocator::getUsed()
	{
		return m_used;
	}

	std::size_t Allocator::getPeak()
	{
		return m_peak;
	}
}
