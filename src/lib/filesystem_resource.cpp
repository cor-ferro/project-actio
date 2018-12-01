#include "filesystem_resource.h"
#include "utils.h"
#include "boost/filesystem/fstream.hpp"


FilesystemResource::FilesystemResource(const Path& path) : Resource(path) {};

void FilesystemResource::load() {
    utils::getFileContents(path, data, &size);
    loaded = true;
}