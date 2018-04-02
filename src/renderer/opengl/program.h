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

#include <sys/inotify.h>

namespace renderer {
    namespace Opengl {

        struct Program {
            typedef std::unordered_map<std::string, GLuint> ProgramSubroutines;

            enum Program_Flags {
                Watch_Changes = 0x1
            };

            enum ShaderType {
                FragmentShader = GL_FRAGMENT_SHADER,
                VertexShader = GL_VERTEX_SHADER
            };

            struct WatchDescriptor {
                int fd = -1;
                int vertexFd = -1;
                int fragmentFd = -1;
                int geometryFd = -1;
            };

            Program();

            explicit Program(std::string);

            ~Program();

            void init(std::string name, uint flags = 0x0);

            void init(std::string name, Path path, uint flags = 0x0);

            void initShader();

            void use();

            void nouse();

            GLint getUniformLoc(const char *) const;

            GLint getUniformCacheLoc(std::string locationName) const;

            GLint getUniformCacheLoc(Opengl::Uniform::Common &uniform) const;

            GLuint getSubroutineIndex(unsigned int &shaderType, std::string soubroutineName);

            GLuint getSubroutineCacheIndex(unsigned int &shaderType, std::string soubroutineName);

            template<typename T>
            void set(const std::string &name, const T &ref);

            template<typename T>
            void set(Opengl::Uniform::Common uniform, const T &ref);

            void setFloat(const std::string &, const float &) const;

            void setInt(const std::string &, const int &) const;

            void setMat(const std::string &, const mat4 &) const;

            void setMat(const std::string &, const mat3 &) const;

            void setMat(const std::string &, const mat2 &) const;

            void setMat(const std::string &, const std::vector<mat4> *mats) const;

            void setMat(const std::string &, const int size, const mat4 *mats) const;

            void setMat(const std::string &, const int size, const float *numbers) const;

            void setVec(const std::string &, const vec2 &) const;

            void setVec(const std::string &, const vec3 &) const;

            void setVec(const std::string &, const vec4 &) const;

            void setMat(Opengl::Uniform::Common, const mat4 &) const;

            void setMat(Opengl::Uniform::Common, const std::vector<mat4> *) const;

            void setFloat(Opengl::Uniform::Common, const float &) const;

            void setVec(Opengl::Uniform::Common, const glm::vec2 &) const;

            void setVec(Opengl::Uniform::Common, const glm::vec3 &) const;

            void setVec(Opengl::Uniform::Common, const glm::vec4 &) const;

            void enableSubroutine(unsigned int shaderType, std::string &subroutinName);

            void enableVertexSubroutine(std::string f, std::string subroutineName);

            void enableFragmentSubroutine(std::string f, std::string subroutineName);

            void bindBlock(const char *blockName, int point);

            void initUniformCache(std::vector<std::string> locations);

            void initUniformCache(std::map<Opengl::Uniform::Common, std::string> locations);

            void checkShadersUpdate();

            void defineFragmentSubroutines(std::string uniform, std::vector<std::string> functions);

            void defineVertexSubroutines(std::string uniform, std::vector<std::string> functions);

            const GLuint getHandle() const;

            const bool isSuccess() const;

            GLuint handle;

            Shader *vertexShader;
            Shader *fragmentShader;
            Shader *geometryShader;

            std::string name;
        private:
            void setupWatch();

            void removeWatch();

            bool compile();

            WatchDescriptor *watcher;
            bool isUsed;
            bool success;
            std::unordered_map<std::string, GLint> uniformIndexCache;
            std::map<short int, GLint> uniformIndexCache2;
            std::unordered_map<std::string, GLuint> subroutineVertexIndexCache;
            std::unordered_map<std::string, GLuint> subroutineFragmentIndexCache;

            ProgramSubroutines vertexSubroutines;
            ProgramSubroutines fragmentSubroutines;

            GLsizei numActiveVertexUniforms = 0;
            GLsizei numActiveFragmentUniforms = 0;
        };

    }
}

#endif /* PROGRAM_H_ */
