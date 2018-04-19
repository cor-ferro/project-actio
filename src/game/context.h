//
// Created by demitriy on 4/20/18.
//

#ifndef ACTIO_CONTEXT_H
#define ACTIO_CONTEXT_H

#include "../lib/types.h"

namespace game {
    struct Context {
        float screenWidth;
        float screenHeight;
        float windowWidth;
        float windowHeight;
        vec2 mousePosition;
        vec3 mouseWorldPosition;
    };
}

#endif //ACTIO_CONTEXT_H
