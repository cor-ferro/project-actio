#ifndef ACTIO_SKIN_BUILDER_H
#define ACTIO_SKIN_BUILDER_H

#include "model.h"
#include "skin.h"
#include <assimp/scene.h>

class SkinBuilder {
public:
    static void FromAi(Skin *skin, aiScene const *scene);

    static void createSkeleton(const aiNode *node, RawSkeleton *rawSkeleton);

    static void createJoints(const aiNode *node, RawSkeleton::Joint *joint);
};

#endif //ACTIO_SKIN_BUILDER_H
