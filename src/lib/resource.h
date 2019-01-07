#ifndef ACTIO_LIB_RESOURCE_H
#define ACTIO_LIB_RESOURCE_H

#include <string>
#include "path.h"


class Resource {
public:
    Resource();

    virtual ~Resource() = default;

    virtual const void *const get() const = 0;

    const size_t& getSize();

    virtual void load() = 0;

    bool isLoaded();

protected:
    mutable bool loaded = false;

    mutable size_t size = 0;
};

#endif //ACTIO_LIB_RESOURCE_H
