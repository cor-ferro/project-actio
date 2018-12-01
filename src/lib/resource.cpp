#include "resource.h"

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

const Path &Resource::getPath() const {
    return path;
}