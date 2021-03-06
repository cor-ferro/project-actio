#ifndef GAME_COMPONENTS_TRANSFORM_H_
#define GAME_COMPONENTS_TRANSFORM_H_

#include <glm/glm.hpp>
#include <foundation/PxTransform.h>
#include "../../core/object3D.h"

namespace game {
    namespace components {
        struct Transform : Object3D {
            Transform() = default;

            explicit Transform(const glm::vec3 &newPosition) : Object3D() {
                position = newPosition;
            }

            Transform(const glm::vec3 &newPosition, const glm::quat &newRotation, const glm::vec3 &newScale) : Object3D() {
                position = newPosition;
                quaternion = newRotation;
                scale = newScale;
            }

            Transform(const glm::vec3 &newPosition, const glm::vec4 &newRotation, const glm::vec3 &newScale)
                    : Object3D() {
                position = newPosition;

                quaternion.x = newRotation[0];
                quaternion.y = newRotation[1];
                quaternion.z = newRotation[2];
                quaternion.w = newRotation[3];

                scale = newScale;
            }

            explicit Transform(const physx::PxTransform &transform) : Object3D() {
                setPosition(transform.p);
            }

            bool dirty = false;
        };
    }
}

#endif
