#ifndef ACTIO_CORE_SKIN_H
#define ACTIO_CORE_SKIN_H

#include <unordered_map>
#include <ozz/base/memory/allocator.h>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include "../animation/animation.h"
#include "../core/mesh.h"
#include "../animation/process.h"
#include "../resources/assimp_resource.h"

using ozz::animation::offline::RawSkeleton;

class SkinBuilder;

class Skin {
public:
    friend class SkinBuilder;

    Skin() = default;

    ~Skin();

    void processAnimation(double dt);

    void processAnimation();

    mat4 *getTransforms();

    int getNumBones();

    std::unordered_map<std::string, uint> getNodeIndexes();

    bool canProcessAnimation();

    void setCurrentAnimation(std::string animationName);

    void setSecondaryAnimation(std::string animationName);

    animation::Animation *getAnimation(std::string name);

    animation::AnimationHandler animSamplers;

private:
    ozz::animation::Skeleton *skeleton = nullptr;
    animation::Sampler primarySampler;
    animation::Sampler secondarySampler;
    animation::Handler *animHandler = nullptr;
    std::unordered_map<std::string, animation::Animation *> animations;
};

#endif //ACTIO_CORE_SKIN_H
