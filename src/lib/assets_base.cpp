#include "assets_base.h"

namespace assets {
    /**
 * -------------------- base --------------------
 */
    BaseAsset::BaseAsset(Resource *resource) : resource(resource) {};

    BaseAsset::~BaseAsset() {
        if (resource != nullptr) {
            delete resource;
            resource = nullptr;
        }
    }

    const Resource *BaseAsset::getResource() {
        return resource;
    }
}