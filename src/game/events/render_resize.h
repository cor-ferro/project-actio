//
// Created by demitriy on 4/22/18.
//

#ifndef ACTIO_RENDER_RESIZE_H
#define ACTIO_RENDER_RESIZE_H

#include "../../renderer/params.h"

namespace game {
    namespace events {
        struct RenderResize {
            explicit RenderResize(renderer::Dimension width, renderer::Dimension height)
                    : width(width)
                    , height(height) {}

            renderer::Dimension width;
            renderer::Dimension height;
        };
    }
}

#endif //ACTIO_RENDER_RESIZE_H
