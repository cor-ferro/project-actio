//
// Created by demitriy on 3/29/18.
//

#ifndef ACTIO_TARGET_H
#define ACTIO_TARGET_H

#include <glm/glm/vec3.hpp>

namespace game {
    namespace components {
        struct Target {
            explicit Target(glm::vec3 newTarget) : target(newTarget) {}

            Target(float x, float y, float z) : target(glm::vec3(x, y, z)) {};

            glm::vec3 target;
        };
    }
}

#endif //ACTIO_TARGET_H
