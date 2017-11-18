#include "animation.h"

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

        AnimKeyPosition key;
        key.time = nodeAnim->mPositionKeys[i].mTime;
        key.value = vec3(
            nodeAnim->mPositionKeys[i].mValue.x,
            nodeAnim->mPositionKeys[i].mValue.y,
            nodeAnim->mPositionKeys[i].mValue.z
        );

        positions.insert(std::pair<double, AnimKeyPosition>(key.time, key));
    }

    for (int i = 0; i < nodeAnim->mNumRotationKeys; i++) {
        AnimKeyRotation key;
        key.time = nodeAnim->mRotationKeys[i].mTime;
        key.value = quat(
            nodeAnim->mRotationKeys[i].mValue.w, // @todo: в примере был первым по счету
            nodeAnim->mRotationKeys[i].mValue.x, 
            nodeAnim->mRotationKeys[i].mValue.y, 
            nodeAnim->mRotationKeys[i].mValue.z
        );
        
        rotations.insert(std::pair<double, AnimKeyRotation>(key.time, key));
    }

    for (int i = 0; i < nodeAnim->mNumScalingKeys; i++) {
        AnimKeyScale key;
        key.time = nodeAnim->mScalingKeys[i].mTime;
        key.value = vec3(
            nodeAnim->mScalingKeys[i].mValue.x, 
            nodeAnim->mScalingKeys[i].mValue.y, 
            nodeAnim->mScalingKeys[i].mValue.z
        );
        
        scalings.insert(std::pair<double, AnimKeyScale>(key.time, key));
    }
}

const AnimKeyPosition& NodeAnimation::findPosition(double time) const
{
    auto it = positions.lower_bound(time);

    return it == positions.end() 
        ? positions.begin()->second
        : it->second;
}

const AnimKeyRotation& NodeAnimation::findRotation(double time) const
{
    auto it = rotations.lower_bound(time);

    return it == rotations.end()
        ? rotations.begin()->second
        : it->second;
}

const AnimKeyScale& NodeAnimation::findScale(double time) const
{
    auto it = scalings.lower_bound(time);

    return it == scalings.end()
        ? scalings.begin()->second
        : it->second;
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

    // if (name == "idle")
    // {
    //     for (auto it = nodeAnimations.begin(); it != nodeAnimations.end(); it++)
    //     {
    //         const NodeAnimation * na = it->second.get();
    //         for (auto it2 = na->positions.begin(); it2 != na->positions.end(); it2++)
    //         {
    //             // console::info("idle ", na->getName(), " ", it2->second.time);
    //         }
    //     }
    // }
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