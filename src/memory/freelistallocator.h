#ifndef SRC_MEMORY_FREELISTALLOCATOR_H_
#define SRC_MEMORY_FREELISTALLOCATOR_H_

#include "allocator.h"
#include "singlylinkedlist.h"
#include "utils.h"
#include <stdlib.h>
#include <cassert>
#include <limits>
#include <algorithm>
#include "../lib/console.h"

namespace memory {
	class FreeListAllocator : public Allocator {
	public:
		enum PlacementPolicy {
			FIND_FIRST,
			FIND_BEST
		};

	private:
		struct FreeHeader {
			std::size_t blockSize;
		};
		struct AllocationHeader {
			std::size_t blockSize;
			char padding;
		};

		typedef SinglyLinkedList<FreeHeader>::Node Node;


		void* m_start_ptr = nullptr;
		PlacementPolicy m_pPolicy;
		SinglyLinkedList<FreeHeader> m_freeList;

	public:
		FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy);

		virtual ~FreeListAllocator();

		virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;

		virtual void Free(void* ptr) override;

		virtual void Init() override;

		virtual void Reset();
	private:
		FreeListAllocator(FreeListAllocator &freeListAllocator);

		void Coalescence(Node* prevBlock, Node * freeBlock);

		void Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
		void FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
		void FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
	};
}

#endif /* SRC_MEMORY_FREELISTALLOCATOR_H_ */
