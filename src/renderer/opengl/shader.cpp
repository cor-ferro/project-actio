#include "shader.h"

namespace renderer {
    namespace Opengl {
        Shader::Shader(ShaderType type)
                : success(false)
                , handle(0)
                , type(type) {}

        Shader::Shader(ShaderType type, renderer::ShaderContent newContent) : success(false), handle(0), type(type) {
            content = newContent;
        }

        void Shader::create() {
            if (handle != 0) {
                console::warn("shader already created %i", handle);
            }

            switch (type) {
                case VERTEX:
                    handle = glCreateShader(GL_VERTEX_SHADER);
                    break;
                case FRAGMENT:
                    handle = glCreateShader(GL_FRAGMENT_SHADER);
                    break;
                case GEOMETRY:
                    handle = glCreateShader(GL_GEOMETRY_SHADER);
                    break;
            }
        }

        void Shader::setSource() {
            const char *ptr = content.get();
            glShaderSource(handle, 1, &ptr, nullptr);
        }

        bool Shader::compile() {
            setSource();

            GLint shaderStatusSuccess;
            GLchar infoLog[512];

            glCompileShader(handle);
            glGetShaderiv(handle, GL_COMPILE_STATUS, &shaderStatusSuccess);

            if (!shaderStatusSuccess) {
                glGetShaderInfoLog(handle, 512, nullptr, infoLog);
                console::err("Shader error:");
                console::err(infoLog);
                success = false;
            } else {
                success = true;
            }

            return success;
        }

        GLuint Shader::getHandle() {
            return handle;
        }

        void Shader::freeSources() {
            if (handle >= 0) {
                glDeleteShader(handle);
                handle = 0;
            }
        }

    }
}

