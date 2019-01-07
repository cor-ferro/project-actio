#include "filesystem_resource.h"
#include "utils.h"
#include "boost/filesystem/fstream.hpp"


FilesystemResource::FilesystemResource(const Path& path) : Resource(), path(path) {};

void FilesystemResource::load() {
    utils::getFileContents(path, &data, &size);
    loaded = true;
}

const Path &FilesystemResource::getPath() const {
    return path;
}

const void *const FilesystemResource::get() const {
    if (!loaded) {
        utils::getFileContents(path, &data, &size);
        loaded = true;
    }

    if (data.empty()) {
        return nullptr;
    }

    return static_cast<const void*>(data.c_str());
}
