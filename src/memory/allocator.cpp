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
}
