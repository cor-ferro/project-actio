//
// Created by demitriy on 10/7/18.
//

#ifndef ACTIO_RENDERER_FORWARD_SHADING_H
#define ACTIO_RENDERER_FORWARD_SHADING_H

#include "shading.h"
#include "../../lib/console.h"

namespace renderer {
    namespace Opengl {
        class ForwardShading : public Shading {
        public:
            ForwardShading() = default;

            ~ForwardShading() override = default;

            void init(Context *context, const renderer::Params &renderParams) override {}

            void process(const FrameContext &frameContext) override {
                console::warn("opengl forward shading not implemented");
            }
        };
    }
}

#endif //ACTIO_RENDERER_FORWARD_SHADING_H
