//
// Created by demitriy on 10/11/18.
//

#ifndef ACTIO_RENDERER_OPENGL_SHADING_H
#define ACTIO_RENDERER_OPENGL_SHADING_H

#include "context.h"
#include "../params.h"
#include "../frame_context.h"

namespace renderer {
    namespace Opengl {
        class Shading {
        public:
            virtual ~Shading() = 0;

            virtual void init(Context *context, const renderer::Params &renderParams) = 0;

            virtual void process(const FrameContext &frameContext) = 0;
        };
    }
}

#endif //ACTIO_RENDERER_OPENGL_SHADING_H
