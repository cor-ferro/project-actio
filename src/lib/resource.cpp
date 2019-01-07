#include "resource.h"

Resource::Resource() {

}

bool Resource::isLoaded() {
    return loaded;
}

const size_t& Resource::getSize() {
    return size;
}
