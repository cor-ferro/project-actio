#include "assets_loader.h"

namespace assets {
    Loader::Loader(const std::string &entryPoint) : entryPoint(entryPoint) {}

    Loader::Loader(const Path &entryPoint) : entryPoint(entryPoint.string()) {}

    Resource *Loader::load(const Path &path) {
        Resource *resource = nullptr;

        Path fullPath = createPath(entryPoint) /= path;

        if (boost::filesystem::exists(fullPath)) {
            resource = new FilesystemResource(fullPath);
        } else if (false) {
            resource = new PackageResource(path); // @todo
        }

        return resource;
    };
}