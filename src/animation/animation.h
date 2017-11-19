#ifndef NODE_ANIMATION_H_
#define NODE_ANIMATION_H_

#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <assimp/scene.h>
#include <cmath>
#include <limits>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "../resources/resources.h"
#include "../lib/console.h"
#include "../lib/types.h"

struct AnimKey {
    double time;
    mat4 value;
};

struct NodeAnimation {
    NodeAnimation(const NodeAnimation& other);
    NodeAnimation(const aiNodeAnim *);
    const AnimKey findPosition(double time, bool interpolate) const;
    const AnimKey findRotation(double time, bool interpolate) const;
    const AnimKey findScale(double time, bool interpolate) const;
    void setName(std::string name);
    void setName(const char * name);
    std::string getName() const;
private:
    std::string name;
    std::map<double, AnimKey> positions;
    std::map<double, AnimKey> rotations;
    std::map<double, AnimKey> scalings;
};

struct Animation {
    Animation(const Animation& other);
    Animation(aiAnimation *);

    const NodeAnimation * findNode(std::string name) const;
    std::string getName() const;
    double getDuration() const;
    double getDurationMs() const;
    double getTicksPerSecond() const;
private:
    std::string name;
    double duration;
    double ticksPerSecond;
    double durationMs;
    std::unordered_map<std::string, std::shared_ptr<const NodeAnimation>> nodeAnimations;
};

struct AnimationProcess {
    AnimationProcess();
    void tick(double ms);
    void setAnimation(const Animation * newAnimation);
    const Animation * getAnimation() const;
    inline const std::string getName() const;
    bool isSet();
    double getCurrentTime();
    double getCurrentTick();
private:
    const Animation * animation;
    double currentTime;
};

#endif
