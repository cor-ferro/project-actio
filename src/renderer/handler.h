//
// Created by demitriy on 7/27/18.
//

#ifndef ACTIO_RENDERER_HANDLER_H
#define ACTIO_RENDERER_HANDLER_H

namespace renderer {
    struct GeometryHandle {
        GeometryHandle() = default;
        virtual ~GeometryHandle() = default;
    };

    struct MaterialHandle {
        MaterialHandle() = default;
        virtual ~MaterialHandle() = default;
    };

    struct TextureHandle {
        TextureHandle() = default;
        virtual ~TextureHandle() = default;
    };
}

#endif //ACTIO_RENDERER_HANDLER_H
