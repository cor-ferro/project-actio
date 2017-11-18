#ifndef NODE_ANIMATION_H_
#define NODE_ANIMATION_H_

#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <assimp/scene.h>
#include "../resources/resources.h"
#include "../lib/console.h"
#include "../lib/types.h"

struct AnimKeyPosition {
    double time;
    vec3 value;
};

struct AnimKeyRotation {
    double time;
    quat value;
};

struct AnimKeyScale {
    double time;
    vec3 value;
};

struct NodeAnimation {
    NodeAnimation(const NodeAnimation& other);
    NodeAnimation(const aiNodeAnim *);
    const AnimKeyPosition& findPosition(double time) const;
    const AnimKeyRotation& findRotation(double time) const;
    const AnimKeyScale& findScale(double time) const;
    void setName(std::string name);
    void setName(const char * name);
    std::string getName() const;
private:
    std::string name;
    std::map<double, AnimKeyPosition> positions;
    std::map<double, AnimKeyRotation> rotations;
    std::map<double, AnimKeyScale> scalings;
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
