//
// Created by demitriy on 3/29/18.
//

#ifndef ACTIO_CAMERA_H
#define ACTIO_CAMERA_H

#include "../../cameras/camera.h"

namespace game {
    namespace components {
        struct Camera : PerspectiveCamera {
            Camera(float fov, float aspect, float near, float far) : PerspectiveCamera(fov, aspect, near, far) {};
        };
    }
}

#endif //ACTIO_CAMERA_H
