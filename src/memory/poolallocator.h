#ifndef SRC_MEMORY_POOLALLOCATOR_H_
#define SRC_MEMORY_POOLALLOCATOR_H_


#include "allocator.h"
#include "stacklinkedlist.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <algorithm>
#include "../lib/console.h"

namespace memory {
	class PoolAllocator : public Allocator {
	private:
		struct  FreeHeader{
		};
		typedef StackLinkedList<FreeHeader>::Node Node;
		StackLinkedList<FreeHeader> m_freeList;

		void * m_start_ptr = nullptr;
		std::size_t m_chunkSize;
	public:
		PoolAllocator(const std::size_t totalSize, const std::size_t chunkSize);

		virtual ~PoolAllocator();

		virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;

		virtual void Free(void* ptr) override;

		virtual void Init() override;

		virtual void Reset();
	private:
		PoolAllocator(PoolAllocator &poolAllocator);

	};
}


#endif /* SRC_MEMORY_POOLALLOCATOR_H_ */
