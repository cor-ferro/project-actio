//
// Created by demitriy on 10/6/18.
//

#ifndef ACTIO_RENDERER_OPENGLHANDLE_H
#define ACTIO_RENDERER_OPENGLHANDLE_H

#include <glad/include/glad/glad.h>
#include "../handle.h"
#include "../../core/geometry.h"

namespace renderer {
    namespace Opengl {
        class GeometryHandle : public virtual renderer::GeometryHandle {
        public:
            explicit GeometryHandle(::Geometry *geometry);
            ~GeometryHandle() override;

            void reset(::Geometry *geometry);

            bool ready = false;
            GLuint vao = 0;
            GLuint vbo = 0;
            GLuint ebo = 0;
            GLenum draw = GL_STATIC_DRAW;
            ::Geometry *geometry;
        private:
            void setup();
            void destroy();
        };

        class MaterialHandle : public virtual renderer::MaterialHandle {
        public:
            ~MaterialHandle() override = default;

            bool ready = false;
            GLuint programId = 0;
            GLuint textureId = 0;
        };

        class TextureHandle : public virtual renderer::TextureHandle {
        public:
            ~TextureHandle() override = default;

            bool ready = false;
            GLenum target = GL_TEXTURE_2D;
            GLuint textureId = 0;
        };
    }
}

#endif //ACTIO_RENDERER_OPENGLHANDLE_H
