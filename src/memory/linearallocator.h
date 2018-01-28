#ifndef SRC_MEMORY_LINEARALLOCATOR_H_
#define SRC_MEMORY_LINEARALLOCATOR_H_

#include "allocator.h"
#include "utils.h"
#include <stdlib.h>
#include <cassert>
#include <algorithm>
#include "../lib/console.h"

namespace memory {
	class LinearAllocator : public Allocator {
	protected:
		void* m_start_ptr = nullptr;
		std::size_t m_offset;
	public:
		LinearAllocator(const std::size_t totalSize);

		virtual ~LinearAllocator();

		virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;

		virtual void Free(void* ptr) override;

		virtual void Init() override;

		virtual void Reset();
	private:
		LinearAllocator(LinearAllocator &linearAllocator);
	};
}


#endif /* SRC_MEMORY_LINEARALLOCATOR_H_ */
