#include "skin.h"

Skin::~Skin() {
    if (skeleton != nullptr) {
        ozz::memory::Allocator *allocator = ozz::memory::default_allocator();

        for (auto &pair : animations) {
            delete pair.second;
            pair.second = nullptr;
        }

        animations.erase(animations.begin(), animations.end());

        allocator->Delete(skeleton);
        skeleton = nullptr;
    }

    delete animHandler;
}

void Skin::processAnimation(double dt) {
    if (primarySampler.isCanUsed()) {
        primarySampler.tick(dt / 1000.0);
    }

    if (secondarySampler.isCanUsed()) {
        secondarySampler.tick(dt / 1000.0);
    }

    for (auto &sampler : animSamplers.getSamplers()) {
        sampler.tick(dt / 1000.0);
    }

    processAnimation();
}

void Skin::processAnimation() {
    animation::process(animSamplers, animHandler);
}

mat4 *Skin::getTransforms() {
    return animHandler->getTransforms();
}

int Skin::getNumBones() {
    return skeleton->num_joints();
}

std::unordered_map<std::string, uint> Skin::getNodeIndexes() {
    std::unordered_map<std::string, uint> map;

    ozz::Range<const char *const> jointNames = skeleton->joint_names();

    for (uint jIndex = 0; jIndex < jointNames.Count(); jIndex++) {
        map.insert({std::string(jointNames[jIndex]), jIndex});
    }

    return map;
};

bool Skin::canProcessAnimation() {
    return primarySampler.isCanUsed();
}

void Skin::setCurrentAnimation(std::string animationName) {
    auto it = animations.find(animationName);

    if (it != animations.end()) {
        primarySampler.animation = it->second;
    }
}

void Skin::setSecondaryAnimation(std::string animationName) {
    auto it = animations.find(animationName);

    if (it != animations.end()) {
        secondarySampler.animation = it->second;
    }
}

animation::Animation *Skin::getAnimation(std::string name) {
    auto it = animations.find(name);
    if (it == animations.end()) {
        return nullptr;
    }

    return it->second;
}