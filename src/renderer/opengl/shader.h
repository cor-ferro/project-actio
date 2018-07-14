#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../lib/console.h"
#include "../shader_content.h"

namespace renderer {
    namespace Opengl {
        struct Shader {
            enum ShaderType {
                VERTEX = 1,
                FRAGMENT = 2,
                GEOMETRY = 3
            };

            explicit Shader(ShaderType type);

            Shader(ShaderType type, renderer::ShaderContent content);

            ~Shader() = default;

            void create();

            bool compile();

            void freeSources();

            GLuint getHandle();

        private:
            renderer::ShaderContent content;

            void setSource();

            ShaderType type;
            GLuint handle;
            bool success;
        };
    }
}

#endif
