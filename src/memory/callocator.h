#ifndef SRC_MEMORY_CALLOCATOR_H_
#define SRC_MEMORY_CALLOCATOR_H_

#include "allocator.h"
#include <stdlib.h>

namespace memory {
	class CAllocator : public Allocator {
	public:
		CAllocator();

		virtual ~CAllocator();

		virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;

		virtual void Free(void* ptr) override;

		virtual void Init() override;
	};
}



#endif /* SRC_MEMORY_CALLOCATOR_H_ */
