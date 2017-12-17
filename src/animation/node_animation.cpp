#include "node_animation.h"

double inf = std::numeric_limits<double>::infinity();

inline vec3 mixKeys(const vec3& v1, const vec3& v2, float& delta) {
    return glm::mix(v1, v2, delta);
}

inline quat mixKeys(const quat& q1, const quat& q2, float& delta) {
    return glm::slerp(q1, q2, delta);
}

template <class T1, typename T2>
T2 interpolateKeys(const T1& key1, const T1& key2, double time) {
    if (key1.time == key2.time) {
        return key1.value;
    }

    double diff = time - key1.time;

    T2 value;

    if (diff > 0.0) {
        float delta = (float)(1.0 - (key2.time - time) / (key2.time - key1.time));

        value = mixKeys(key1.value, key2.value, delta);
    } else if (diff < 0.0) {
        value = key1.value;
    } else {
        value = key1.value;
    }

    return value;
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
        AnimKeyPosition key;
        key.time = nodeAnim->mPositionKeys[i].mTime;
        key.value = libAi::toNativeType(nodeAnim->mPositionKeys[i].mValue);

        positions.insert({ key.time, key });
    }

    for (int i = 0; i < nodeAnim->mNumRotationKeys; i++) {
        AnimKeyRotation key;
        key.time = nodeAnim->mRotationKeys[i].mTime;
        key.value = libAi::toNativeType(nodeAnim->mRotationKeys[i].mValue);
        
        rotations.insert({ key.time, key });
    }

    for (int i = 0; i < nodeAnim->mNumScalingKeys; i++) {
        AnimKeyScale key;
        key.time = nodeAnim->mScalingKeys[i].mTime;
        key.value = libAi::toNativeType(nodeAnim->mScalingKeys[i].mValue);
        
        scalings.insert({ key.time, key });
    }
}

const AnimKey NodeAnimation::findKey(double time, bool interpolate) const
{
    AnimKey key;

    key.time = time;

    double t = floor(time);
    auto itPosition = positions.lower_bound(t);
    auto itRotation = rotations.lower_bound(t);
    auto itScale = scalings.lower_bound(t);

    if (itPosition == positions.end()) itPosition = positions.begin();
    if (itRotation == rotations.end()) itRotation = rotations.begin();
    if (itScale == scalings.end()) itScale = scalings.begin();

    if (interpolate == true) {
        auto itNextPosition = std::next(itPosition, 1);
        auto itNextRotation = std::next(itRotation, 1);
        auto itNextScale = std::next(itScale, 1);

        if (itNextPosition == positions.end()) itNextPosition = itPosition;
        if (itNextRotation == rotations.end()) itNextRotation = itRotation;
        if (itNextScale == scalings.end()) itNextScale = itScale;

        key.position = interpolateKeys<AnimKeyPosition, vec3>(itPosition->second, itNextPosition->second, time);
        key.rotation = interpolateKeys<AnimKeyRotation, quat>(itRotation->second, itNextRotation->second, time);
        key.scale = interpolateKeys<AnimKeyScale, vec3>(itScale->second, itNextScale->second, time);
    } else {
        key.position = itPosition->second.value;
        key.rotation = itRotation->second.value;
        key.scale = itScale->second.value;
    }

    return key;
}

const AnimKeyPosition NodeAnimation::findPosition(double time, bool interpolate) const
{
    auto it = positions.lower_bound(floor(time));

    if (it == positions.end()) return positions.begin()->second;

    if (interpolate == true) {
        auto nextIt = std::next(it, 1);
        if (nextIt == positions.end()) return it->second;

        // return interpolateKey(it->second, nextIt->second, time);
    }

    return it->second;
}

const AnimKeyRotation NodeAnimation::findRotation(double time, bool interpolate) const
{
    auto it = rotations.lower_bound(floor(time));

    if (it == rotations.end()) return rotations.begin()->second;

    if (interpolate == true) {
        auto nextIt = std::next(it, 1);
        if (nextIt == rotations.end()) return it->second;

        // return interpolateKey(it->second, nextIt->second, time);
    }

    return it->second;
}

const AnimKeyScale NodeAnimation::findScale(double time, bool interpolate) const
{
    auto it = scalings.lower_bound(floor(time));

    if (it == scalings.end()) return scalings.begin()->second;

    if (interpolate == true) {
        auto nextIt = std::next(it, 1);
        if (nextIt == scalings.end()) return it->second;

        // return interpolateKey(it->second, nextIt->second, time);
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