#include "animation.h"

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