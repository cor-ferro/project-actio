//
// Created by demitriy on 3/24/18.
//

#ifndef ACTIO_CONTROLLED_H
#define ACTIO_CONTROLLED_H

#include "characterkinematic/PxController.h"

namespace game {
    namespace components {
        using namespace physx;

        struct Controlled {
            Controlled(PxControllerManager* const manager) {
        		PxCapsuleControllerDesc desc;
                controller = manager->createController(desc);
            }

            PxController* controller;
        };
    }
}

#endif //ACTIO_CONTROLLED_H
