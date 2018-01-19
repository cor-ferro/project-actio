#ifndef NODE_ANIMATION_H_
#define NODE_ANIMATION_H_

#include <map>
#include <assimp/scene.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../lib/types.h"
#include "../lib/console.h"
#include "../lib/assimp.h"

struct AnimKey {
    double time;
    vec3 position;
    quat rotation;
    vec3 scale;
};

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
    const AnimKey findKey(double time, bool interpolate) const;
    const AnimKeyPosition findPosition(double time, bool interpolate) const;
    const AnimKeyRotation findRotation(double time, bool interpolate) const;
    const AnimKeyScale findScale(double time, bool interpolate) const;
    void setName(std::string name);
    void setName(const char * name);
    std::string getName() const;
private:
    std::string name;
    std::map<double, AnimKeyPosition> positions;
    std::map<double, AnimKeyRotation> rotations;
    std::map<double, AnimKeyScale> scalings;
};

#endif