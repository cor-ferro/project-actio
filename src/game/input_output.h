//
// Created by demitriy on 11/17/18.
//

#ifndef ACTIO_INPUT_OUTPUT_H
#define ACTIO_INPUT_OUTPUT_H

#include <memory>
#include "../lib/input_manager.h"
#include "../renderer/renderer.h"
#include "../renderer/window_context.h"

namespace game {
    struct InputOutput {
        std::shared_ptr<InputManager> input;
        std::shared_ptr<renderer::Renderer> renderer;
        std::shared_ptr<WindowContext> windowContext;
    };
}

#endif //ACTIO_INPUT_OUTPUT_H
