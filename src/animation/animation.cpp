#include "animation.h"

double inf = std::numeric_limits<double>::infinity();

AnimKey interpolateKey(const AnimKey& key1, const AnimKey& key2, double time) {
    if (key1.time == key2.time) {
        return key1;
    }

    double diff = time - key1.time;

    AnimKey interpolatedKey;

    if (diff > 0.0) {
        double delta = 1.0 - (key2.time - time) / (key2.time - key1.time);

        interpolatedKey.time = time;
        interpolatedKey.value = glm::mix(key1.value, key2.value, delta);

        return interpolatedKey;
    } else if (diff < 0.0) {
        return key1;
    } else {
        return key1;
    }

    return interpolatedKey;
}

NodeAnimation::NodeAnimation(const NodeAnimation& other)
{
    name = other.name;
    positions = other.positions;
    rotations = other.rotations;
    scalings = other.scalings;
}

NodeAnimation::NodeAnimation(const aiNodeAnim * nodeAnim)
{
    setName(nodeAnim->mNodeName.C_Str());

    // console::info("node animation", getName());

    for (int i = 0; i < nodeAnim->mNumPositionKeys; i++) {
        // console::info("++++++ NodeAnimation position time", nodeAnim->mPositionKeys[i].mTime);

        AnimKey key;
        key.time = nodeAnim->mPositionKeys[i].mTime;
        key.value = glm::translate(mat4(1.0), vec3(
            nodeAnim->mPositionKeys[i].mValue.x,
            nodeAnim->mPositionKeys[i].mValue.y,
            nodeAnim->mPositionKeys[i].mValue.z
        ));

        positions.insert(std::pair<double, AnimKey>(key.time, key));
    }

    for (int i = 0; i < nodeAnim->mNumRotationKeys; i++) {
        AnimKey key;
        key.time = nodeAnim->mRotationKeys[i].mTime;
        key.value = glm::toMat4(quat(
            nodeAnim->mRotationKeys[i].mValue.w, // @todo: в примере был первым по счету
            nodeAnim->mRotationKeys[i].mValue.x, 
            nodeAnim->mRotationKeys[i].mValue.y, 
            nodeAnim->mRotationKeys[i].mValue.z
        ));
        
        rotations.insert(std::pair<double, AnimKey>(key.time, key));
    }

    for (int i = 0; i < nodeAnim->mNumScalingKeys; i++) {
        AnimKey key;
        key.time = nodeAnim->mScalingKeys[i].mTime;
        key.value = glm::scale(mat4(1.0), vec3(
            nodeAnim->mScalingKeys[i].mValue.x, 
            nodeAnim->mScalingKeys[i].mValue.y, 
            nodeAnim->mScalingKeys[i].mValue.z
        ));
        
        scalings.insert(std::pair<double, AnimKey>(key.time, key));
    }
}

const AnimKey NodeAnimation::findPosition(double time, bool interpolate) const
{
    auto it = positions.lower_bound(floor(time));

    if (it == positions.end()) return positions.begin()->second;

    if (interpolate == true) {
        auto nextIt = std::next(it, 1);
        if (nextIt == positions.end()) return it->second;

        return interpolateKey(it->second, nextIt->second, time);
    }

    return it->second;
}

const AnimKey NodeAnimation::findRotation(double time, bool interpolate) const
{
    auto it = rotations.lower_bound(floor(time));

    if (it == rotations.end()) return rotations.begin()->second;

    if (interpolate == true) {
        auto nextIt = std::next(it, 1);
        if (nextIt == positions.end()) return it->second;

        return interpolateKey(it->second, nextIt->second, time);
    }

    return it->second;
}

const AnimKey NodeAnimation::findScale(double time, bool interpolate) const
{
    auto it = scalings.lower_bound(floor(time));

    if (it == scalings.end()) return scalings.begin()->second;

    if (interpolate == true) {
        auto nextIt = std::next(it, 1);
        if (nextIt == positions.end()) return it->second;

        return interpolateKey(it->second, nextIt->second, time);
    }

    return it->second;
}

void NodeAnimation::setName(std::string newName)
{
    name = newName;
}

void NodeAnimation::setName(const char * newName)
{
    name = std::string(newName);
}

std::string NodeAnimation::getName() const
{
    return name;
}




Animation::Animation(const Animation& other)
{
    name = other.name;
    duration = other.duration;
    durationMs = other.durationMs;
    ticksPerSecond = other.ticksPerSecond;
    nodeAnimations = other.nodeAnimations;
}

Animation::Animation(aiAnimation * animation)
{
    name = std::string(animation->mName.C_Str());
    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond;
    durationMs = (duration / ticksPerSecond) * 1000.0;

    console::info("animation ", name, " ", duration, " ", ticksPerSecond);

    for (int i = 0; i < animation->mNumChannels; i++)
    {
        const aiNodeAnim * channel = animation->mChannels[i];
        const NodeAnimation * nodeAnim = new NodeAnimation(channel);
        std::string nodeName = std::string(channel->mNodeName.C_Str());

        nodeAnimations.insert(
            std::pair<std::string, std::shared_ptr<const NodeAnimation>>(nodeName, std::shared_ptr<const NodeAnimation>(nodeAnim))
        );
    }
}

const NodeAnimation * Animation::findNode(std::string name) const
{
    auto it = nodeAnimations.find(name);

    if (it == nodeAnimations.end()) {
        return nullptr;
    }

    return it->second.get();
}

std::string Animation::getName() const
{
    return name;
}

double Animation::getDuration() const
{
    return duration;
}

double Animation::getDurationMs() const
{
    return durationMs;
}

double Animation::getTicksPerSecond() const
{
    return ticksPerSecond;
}



AnimationProcess::AnimationProcess()
    : animation(nullptr)
    , currentTime(0.0)
{}

void AnimationProcess::tick(double ms) {
    currentTime+= ms;

    if (currentTime > animation->getDurationMs()) {
        currentTime = 0.0;
    }
}

void AnimationProcess::setAnimation(const Animation * newAnimation) {
    animation = newAnimation;
}

const Animation * AnimationProcess::getAnimation() const {
    return animation;
}

inline const std::string AnimationProcess::getName() const {
    return animation->getName();
}

bool AnimationProcess::isSet() {
    return animation != nullptr;
}

double AnimationProcess::getCurrentTime() {
    return currentTime;
}

double AnimationProcess::getCurrentTick() {
    return (currentTime / animation->getDurationMs()) * animation->getDuration();
}