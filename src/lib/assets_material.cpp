#include "assets_material.h"

namespace assets {
    /**
 * -------------------- Material --------------------
 */
    Material::Material(::Material *material) : BaseAsset(nullptr) {
        Material::material.reset(material);
    }

    const std::shared_ptr<::Material> &Material::getMaterial() const {
        return material;
    }
}