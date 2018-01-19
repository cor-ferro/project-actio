#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <assimp/scene.h>
#include <cmath>
#include <limits>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../resources/resources.h"
#include "../lib/console.h"
#include "../lib/types.h"
#include "node_animation.h"

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
