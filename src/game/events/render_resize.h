//
// Created by demitriy on 4/22/18.
//

#ifndef ACTIO_RENDER_RESIZE_H
#define ACTIO_RENDER_RESIZE_H

#include "../../renderer/renderer_types.h"

namespace game {
    namespace events {
        struct RenderResize {
            explicit RenderResize(renderer::ScreenSize width, renderer::ScreenSize height)
                    : width(width)
                    , height(height) {}

            renderer::ScreenSize width;
            renderer::ScreenSize height;
        };
    }
}

#endif //ACTIO_RENDER_RESIZE_H
