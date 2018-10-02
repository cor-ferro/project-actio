#include "assets_base.h"

namespace assets {
    /**
 * -------------------- base --------------------
 */
    BaseAsset::BaseAsset(assets::Resource *resource) : resource(resource) {};

    BaseAsset::~BaseAsset() {
        if (resource != nullptr) {
            delete resource;
            resource = nullptr;
        }
    }

    const assets::Resource *BaseAsset::getResource() {
        return resource;
    }
}