//
// Created by demitriy on 3/24/18.
//

#ifndef ACTIO_CONTROLLED_H
#define ACTIO_CONTROLLED_H

#include "characterkinematic/PxController.h"
#include "characterkinematic/PxControllerManager.h"
#include "characterkinematic/PxBoxController.h"
#include "characterkinematic/PxCapsuleController.h"
#include "../../lib/types.h"

namespace game {
    namespace components {
        using namespace physx;

        struct Controlled {
            enum Direction {
                Left,
                Right
            };

            explicit Controlled(PxController* const mController)
                : controller(mController)
                , disp(vec3(0.0f))
                , deltaX(vec3(0.0f))
                , delta(vec3(0.0f))
                , jumpForce(0.0f)
                , isJump(false)
                {}

            PxController* controller;

            vec3 disp;
            vec3 deltaX;
            vec3 delta;
            float jumpForce;
            bool isJump;
            float angle = 0.0f;
            Direction sideDirection = Direction::Right;
        };
    }
}

#endif //ACTIO_CONTROLLED_H
