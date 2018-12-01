#ifndef ACTIO_ASSETS_BASE_H
#define ACTIO_ASSETS_BASE_H

#include "resource.h"

namespace assets {
    class BaseAsset {
    public:
        explicit BaseAsset(Resource *resource);

        ~BaseAsset();

        const Resource *getResource();

    protected:
        bool isLoaded = false;
        Resource *resource = nullptr;
    };

}

#endif //ACTIO_ASSETS_BASE_H
