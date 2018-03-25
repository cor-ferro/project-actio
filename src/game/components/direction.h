#ifndef GAME_COMPONENTS_DIRECTION_H_
#define GAME_COMPONENTS_DIRECTION_H_

#include <glm/glm.hpp>

namespace game {
    namespace components {
        struct Direction {
            Direction(float x, float y, float z) {
                dir.x = x;
                dir.y = y;
                dir.z = z;
            };

            glm::vec3 dir;
        };
    }
}

#endif
