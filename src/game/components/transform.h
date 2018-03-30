#ifndef GAME_COMPONENTS_TRANSFORM_H_
#define GAME_COMPONENTS_TRANSFORM_H_

#include <glm/glm.hpp>
#include "../../core/object3D.h"

namespace game {
    namespace components {
        struct Transform : Object3D {
            Transform() = default;

            explicit Transform(glm::vec3 newPosition) : Object3D() {
                position = newPosition;
            }

            Transform(glm::vec3 newPosition, glm::quat newRotation, glm::vec3 newScale) : Object3D() {
                position = newPosition;
                quaternion = newRotation;
                scale = newScale;
            }

            Transform(glm::vec3 newPosition, glm::vec4 newRotation, glm::vec3 newScale) : Object3D() {
                position = newPosition;

                quaternion.x = newRotation[0];
                quaternion.y = newRotation[1];
                quaternion.z = newRotation[2];
                quaternion.w = newRotation[3];

                scale = newScale;
            };
        };
    }
}

#endif
