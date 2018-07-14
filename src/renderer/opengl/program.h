#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <glad/glad.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <functional>
#include "uniforms.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "../../lib/console.h"
#include "../../lib/types.h"
#include "../../lib/path.h"
#include "../../app/app.h"
#include "../shader_description.h"

#include <sys/inotify.h>

namespace renderer {
    namespace Opengl {

        struct Program {
            typedef std::unordered_map<std::string, GLuint> ProgramSubroutines;
            typedef std::unordered_map<std::string, GLuint> ProgramUniformSubroutines;

            enum ShaderType {
                FragmentShader = GL_FRAGMENT_SHADER,
                VertexShader = GL_VERTEX_SHADER
            };

            Program();

            explicit Program(renderer::ShaderDescription &);

            ~Program();

            void init();

            void use();

            void nouse();

            GLint getUniformLoc(const char *) const;

            GLint getUniformCacheLoc(std::string locationName) const;

            template<typename T>
            void set(const std::string &name, const T &ref);

            void setFloat(const std::string &, const float &) const;

            void setInt(const std::string &, const int &) const;

            void setMat(const std::string &, const mat4 &) const;

            void setMat(const std::string &, const mat3 &) const;

            void setMat(const std::string &, const mat2 &) const;

            void setMat(const std::string &, const std::vector<mat4> *mats) const;

            void setMat(const std::string &, int size, const mat4 *mats) const;

            void setMat(const std::string &, int size, const float *numbers) const;

            void setVec(const std::string &, const vec2 &) const;

            void setVec(const std::string &, const vec3 &) const;

            void setVec(const std::string &, const vec4 &) const;

            void enableVertexSubroutine(std::string f, std::string subroutineName);

            void enableFragmentSubroutine(std::string f, std::string subroutineName);

            void bindBlock(const char *blockName, GLuint point);

            void initUniformCache();

            const GLuint getHandle() const;

            const bool isSuccess() const;

            const std::string getName();

            GLuint handle;

            std::shared_ptr<Shader> vertexShader;
            std::shared_ptr<Shader> fragmentShader;
            std::shared_ptr<Shader> geometryShader;

            std::string name;
        private:
            bool compile();

            void collectUniforms();

            void collectSubroutines();

            void collectSubroutinesInformation(GLenum shaderType, ProgramUniformSubroutines *, ProgramSubroutines *);

            bool isUsed = false;
            bool success = false;
            std::unordered_map<std::string, GLint> uniformIndexCache;

            ProgramSubroutines vertexSubroutines;
            ProgramSubroutines fragmentSubroutines;
            ProgramSubroutines geometrySubroutines;

            ProgramUniformSubroutines vertexSubroutineUniforms;
            ProgramUniformSubroutines fragmentSubroutineUniforms;
            ProgramUniformSubroutines geometrySubroutineUniforms;
        };

    }
}

#endif /* PROGRAM_H_ */
