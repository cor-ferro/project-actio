#ifndef ACTIO_CORE_MODEL_BUILDER_H
#define ACTIO_CORE_MODEL_BUILDER_H

#include "model.h"
#include "../resources/assimp_resource.h"
#include "../lib/assets.h"

class ModelBuilder {
public:
    static void FromAi(Model *model, Resource::Assimp *assimpResource, Assets *assets);
};

#endif //ACTIO_MODEL_BUILDER_H
