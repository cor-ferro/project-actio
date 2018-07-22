#include "assets_loader.h"

namespace game {
    namespace assets {
        Loader::Loader(const std::string &entryPoint) : entryPoint(entryPoint) {}

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
}