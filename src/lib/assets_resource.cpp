#include "assets_resource.h"
#include "utils.h"

namespace assets {
    Resource::Resource(const Path path) : path(path) {
        data = std::make_shared<std::string>("");
    }

    bool Resource::isLoaded() {
        return loaded;
    }

    const size_t Resource::getSize() {
        return size;
    }

    Resource::Content Resource::get() {
        if (!loaded) {
            load();
        }

        return data;
    }

    void Resource::reset() {
        data.reset();
    }


    FilesystemResource::FilesystemResource(Path path) : Resource(path) {};

    void FilesystemResource::load() {
        utils::getFileContents(path, data, &size);
        loaded = true;
    }


    PackageResource::PackageResource(Path path) : Resource(path) {};

    void PackageResource::load() {}
}