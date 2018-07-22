#include "assets_resource.h"
#include "../lib/utils.h"

namespace game {
    namespace assets {
        Resource::Resource(const Path path) : path(path) {}

        bool Resource::isLoaded() {
            return loaded;
        }

        const size_t Resource::getSize() {
            return size;
        }

        char* Resource::get() {
            if (!loaded) {
                load();
            }

            return data;
        }

        void Resource::reset() {
            if (data != nullptr) {
                delete[] data;
                data = nullptr;
                loaded = false;
            }
        }


        FilesystemResource::FilesystemResource(Path path) : Resource(path) {};

        void FilesystemResource::load() {
            utils::getFileContents(path, data, &size);
            loaded = true;
        }


        PackageResource::PackageResource(Path path) : Resource(path) {};

        void PackageResource::load() {}
    }
}