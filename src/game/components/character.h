//
// Created by demitriy on 4/24/18.
//

#ifndef ACTIO_COMPONENTS_CHARACTER_H
#define ACTIO_COMPONENTS_CHARACTER_H

#include "../../lib/types.h"

namespace game {
    namespace components {
        struct Character {
            Character() = default;

            enum ViewSide {
                Left,
                Right
            };

            ViewSide viewSide = ViewSide::Right;
            vec3 viewDir = vec3(0.0f, 0.0f, 1.0f);
            float viewAngle = 180.0f;

            vec3 motion = vec3(0.0f);
            vec3 motionDelta = vec3(0.0f);
            vec3 motionDeltaX = vec3(0.0f);

            float jump = 0.0f;
            bool isJump = false;
            bool isFreeFall = false;

            float walkSpeed = 1.5f;
            float runSpeed = 1.5f;
            float jumpForce = 1.2f;
        };
    }
}

#endif //ACTIO_COMPONENTS_CHARACTER_H
