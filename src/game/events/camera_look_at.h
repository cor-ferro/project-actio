//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_CAMERA_LOOK_AT
#define ACTIO_CAMERA_LOOK_AT

#include "../../lib/types.h"

namespace game {
    namespace events {
        struct CameraLookAt {
            CameraLookAt(vec3 target) : target(target) {}

            vec3 target;
        };
    }
}

#endif //ACTIO_CAMERA_LOOK_AT
