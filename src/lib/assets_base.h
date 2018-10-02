#ifndef ACTIO_ASSETS_BASE_H
#define ACTIO_ASSETS_BASE_H

#include "assets_resource.h"

namespace assets {
    class BaseAsset {
    public:
        explicit BaseAsset(assets::Resource *resource);

        ~BaseAsset();

        const assets::Resource *getResource();

    protected:
        bool isLoaded = false;
        assets::Resource *resource = nullptr;
    };

}

#endif //ACTIO_ASSETS_BASE_H
