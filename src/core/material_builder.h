#ifndef ACTIO_MATERIAL_BUILDER_H
#define ACTIO_MATERIAL_BUILDER_H

#include "material.h"
#include <memory>
#include "../lib/assets.h"

class MaterialBuilder {
public:
    static void FromAi(std::shared_ptr<Material> &material, const aiMaterial *assimpMaterial, const resources::Assimp *assimpResource, Assets *assets);
};

#endif //ACTIO_MATERIAL_BUILDER_H
