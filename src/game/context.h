//
// Created by demitriy on 4/20/18.
//

#ifndef ACTIO_CONTEXT_H
#define ACTIO_CONTEXT_H

#include <memory>
#include "../lib/types.h"
#include "../lib/input_handler.h"
#include "../lib/assets.h"

namespace game {
    struct Context {
        float screenWidth;
        float screenHeight;
        float windowWidth;
        float windowHeight;
        vec2 mousePosition;
        vec3 mouseWorldPosition;
        InputHandler *input = nullptr;
        std::shared_ptr<Assets> assets;
    };
}

#endif //ACTIO_CONTEXT_H
