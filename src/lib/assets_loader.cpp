#include "assets_loader.h"
#include "filesystem_resource.h"
#include "package_resource.h"
#include "memory_resource.h"

namespace assets {
    Loader::Loader(const std::string &entryPoint) : entryPoint(entryPoint) {}

    Loader::Loader(const Path &entryPoint) : entryPoint(entryPoint.string()) {}

    Resource *Loader::createResource(const Path &path) {
        Path fullPath = createPath(entryPoint) /= path;

        Resource *resource = new FilesystemResource(fullPath);

        return resource;
    }

    Resource *Loader::createResource() {
        return new PackageResource();
    };

    Resource *Loader::createResource(void *const memory) {
        return new MemoryResource(memory, 0);
    }
}