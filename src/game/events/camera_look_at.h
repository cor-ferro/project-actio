//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_KEY_PRESS_H
#define ACTIO_KEY_PRESS_H

#include "../../lib/types.h"

namespace game {
    namespace events {
        struct CameraLookAt {
            CameraLookAt(vec3 target) : target(target) {}

            vec3 target;
        };
    }
}

#endif //ACTIO_KEY_PRESS_H
