#ifndef ACTIO_ASSETS_MATERIAL_H
#define ACTIO_ASSETS_MATERIAL_H

#include "assets_base.h"
#include "../core/material.h"

namespace assets {
    class Material : public BaseAsset {
    public:
        explicit Material(::Material *material);

        const std::shared_ptr<::Material> &getMaterial() const;

    private:
        std::shared_ptr<::Material> material;

        std::vector<std::shared_ptr<Texture>> textures;
    };
}

#endif //ACTIO_ASSETS_MATERIAL_H
