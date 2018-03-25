#ifndef GAME_COMPONENTS_POSITION_H_
#define GAME_COMPONENTS_POSITION_H_

#include <glm/glm.hpp>

namespace game {
    namespace components {
        struct Position {
            Position(float x, float y, float z) {
                pos.x = x;
                pos.y = y;
                pos.z = z;
            };

            Position(glm::vec3& newPos) {
                pos = newPos;
            }

            glm::vec3 pos;
        };
    }
}

#endif