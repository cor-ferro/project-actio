#ifndef ACTIO_ASSIMP_RESOURCE_H
#define ACTIO_ASSIMP_RESOURCE_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "resource.h"
#include "path.h"

class MemoryResource : public Resource {
public:
    explicit MemoryResource(void *const memory, const size_t& size);

    ~MemoryResource() override = default;

    void load() override;

    void *const getMemory() const;

    void *const get() const;
private:
    void *const memory_;
};

#endif //ACTIO_ASSIMP_RESOURCE_H
