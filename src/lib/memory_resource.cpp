#include "memory_resource.h"
#include <assimp/include/assimp/postprocess.h>

MemoryResource::MemoryResource(void *const memory, const size_t &size) : Resource(), memory_(memory) {
    Resource::size = size;
}

void MemoryResource::load() {

}

void *const MemoryResource::getMemory() const {
    return memory_;
}

void *const MemoryResource::get() const {
    return getMemory();
}
