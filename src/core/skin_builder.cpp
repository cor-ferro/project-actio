#include "skin_builder.h"

void SkinBuilder::FromAi(Skin *skin, aiScene const *scene) {
    if (scene->mNumAnimations == 0) {
        return;
    }

    const aiNode *rootNode = scene->mRootNode;

    if (scene->mNumAnimations > 0) {
        ozz::animation::offline::SkeletonBuilder skeletonBuilder;

        RawSkeleton rawSkeleton;

        SkinBuilder::createSkeleton(rootNode, &rawSkeleton);

        skin->skeleton = skeletonBuilder(rawSkeleton);

        for (int i = 0; i < scene->mNumAnimations; i++) {
            auto *animation = new animation::Animation(scene->mAnimations[i], skin->skeleton);

            if (animation->valid()) {
                skin->animations.insert({animation->getName(), animation});
            }
        }

        skin->animHandler = new animation::Handler(skin->skeleton);
    }
}

void SkinBuilder::createSkeleton(const aiNode *node, RawSkeleton *rawSkeleton) {
    rawSkeleton->roots.resize(1);

    RawSkeleton::Joint *root = &rawSkeleton->roots[0];
    root->name = node->mName.C_Str();

    aiMatrix4x4 mat = node->mTransformation;
    aiVector3D translation, scale;
    aiQuaternion rotation;
    mat.Decompose(scale, rotation, translation);

    SkinBuilder::createJoints(node, root);
}

void SkinBuilder::createJoints(const aiNode *node, RawSkeleton::Joint *joint) {
    joint->children.resize(node->mNumChildren);

    for (uint i = 0; i < node->mNumChildren; i++) {
        RawSkeleton::Joint *childJoint = &joint->children[i];
        childJoint->name = node->mChildren[i]->mName.C_Str();

        createJoints(node->mChildren[i], childJoint);
    }
}