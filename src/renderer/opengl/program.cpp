#include "program.h"
#include "utils.h"

namespace renderer {
    namespace gl {
        template<typename T>
        inline void uniform(GLint loc, uint count, const T &mat);

        template<>
        inline void uniform<vec2>(GLint loc, uint count, const glm::vec2 &vec) {
            glUniform2fv(loc, count, glm::value_ptr(vec));
        }

        template<>
        inline void uniform<vec3>(GLint loc, uint count, const glm::vec3 &vec) {
            glUniform3fv(loc, count, glm::value_ptr(vec));
        }

        template<>
        inline void uniform<vec4>(GLint loc, uint count, const glm::vec4 &vec) {
            glUniform4fv(loc, count, glm::value_ptr(vec));
        }

        template<>
        inline void uniform<mat2>(GLint loc, uint count, const glm::mat2 &mat) {
            glUniformMatrix2fv(loc, count, GL_FALSE, &mat[0][0]);
        }

        template<>
        inline void uniform<mat3>(GLint loc, uint count, const glm::mat3 &mat) {
            glUniformMatrix3fv(loc, count, GL_FALSE, &mat[0][0]);
        }

        template<>
        inline void uniform<mat4>(GLint loc, uint count, const glm::mat4 &mat) {
            glUniformMatrix4fv(loc, count, GL_FALSE, &mat[0][0]);
        }


        template<typename T>
        inline void uniform(GLint loc, T value);

        template<>
        inline void uniform<int>(GLint loc, const GLint value) {
            glUniform1i(loc, value);
        }

        template<>
        inline void uniform<float>(GLint loc, const GLfloat value) {
            glUniform1f(loc, value);
        }

    }

    namespace Opengl {
        Program::Program()
                : handle(0)
                , success(false)
                , isUsed(false) {}

        Program::Program(const std::string& name) : Program() {
            this->name = name;
        }

        void Program::init(const renderer::RawProgram &rawProgram) {
            name = rawProgram.getName();

            const renderer::ShaderContent &vertexContent = rawProgram.getVertexShader();
            const renderer::ShaderContent &fragmentContent = rawProgram.getFragmentShader();
            const renderer::ShaderContent &geometryContent = rawProgram.getGeometryShader();

            vertexShader.reset();
            fragmentShader.reset();
            geometryShader.reset();

            if (!vertexContent.empty()) {
                vertexShader.reset(new Shader(Shader::VERTEX, vertexContent));
            }

            if (!fragmentContent.empty()) {
                fragmentShader.reset(new Shader(Shader::FRAGMENT, fragmentContent));
            }

            if (!geometryContent.empty()) {
                geometryShader.reset(new Shader(Shader::GEOMETRY, geometryContent));
            }

            isUsed = false;

            handle = glCreateProgram();

            if (vertexShader) {
                vertexShader->create();
                vertexShader->compile();
                glAttachShader(handle, vertexShader->getHandle());
            }

            if (fragmentShader) {
                fragmentShader->create();
                fragmentShader->compile();
                glAttachShader(handle, fragmentShader->getHandle());
            }

            if (geometryShader) {
                geometryShader->create();
                geometryShader->compile();
                glAttachShader(handle, geometryShader->getHandle());
            }

            if (compile()) {
                success = true;
            }
        }

        bool Program::compile() {
            int success;
            char infoLog[4096];

            glLinkProgram(handle);
            glGetProgramiv(handle, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(handle, 4096, NULL, infoLog);
                std::cout
                        << "ERROR::SHADER::PROGRAM::LINKING_FAILED " << name << std::endl
                        << infoLog
                        << std::endl;
                success = false;
            } else {
                success = true;
            }

            return success;
        }

        Program::~Program() {
            if (handle != 0) {
                glDeleteProgram(handle);
                handle = 0;
            }

            console::info("destroy shader program %s", name);
        }

        const GLuint Program::getHandle() const {
            return handle;
        }

        const bool Program::isSuccess() const {
            return success;
        }

        void Program::use() {
//            if (isUsed) return;
            glUseProgram(handle);
        }

        void Program::nouse() {
            if (isUsed) {
                glUseProgram(0);
                isUsed = false;
            } else {
                console::warn("try no use shader failed");
            }
        }

        void Program::initUniformCache() {
            collectUniforms();
            collectSubroutines();
        }

        void Program::enableVertexSubroutine(std::string uniformName, std::string subroutineName) {
            auto itUniform = vertexSubroutineUniforms.find(uniformName);
            auto itSubroutine = vertexSubroutines.find(subroutineName);

            if (itUniform == vertexSubroutineUniforms.end()) {
//                console::warn("unknown vertex uniform subroutine: %s, %s", name, uniformName);
                return;
            }

            if (itSubroutine == vertexSubroutines.end()) {
                console::warn("unknown vertex subroutine: %s, %s", name, subroutineName);
                return;
            }

            GLint locationOfFunction = itUniform->second;
            GLuint indexFunc1 = itSubroutine->second;

            auto numActiveUniforms = static_cast<GLsizei>(vertexSubroutineUniforms.size());

            std::string name = getName();

            assert(locationOfFunction < numActiveUniforms);

            std::unique_ptr<GLuint> indices(new GLuint[numActiveUniforms]);
            indices.get()[locationOfFunction] = indexFunc1;
            glUniformSubroutinesuiv(GL_VERTEX_SHADER, numActiveUniforms, indices.get());
        }

        void Program::enableFragmentSubroutine(std::string uniformName, std::string subroutineName) {
            auto itUniform = fragmentSubroutineUniforms.find(uniformName);
            auto itSubroutine = fragmentSubroutines.find(subroutineName);

            if (itUniform == fragmentSubroutineUniforms.end()) {
                console::warn("unknown fragment uniform subroutine: %s, %s", name, uniformName);
                return;
            }

            if (itSubroutine == fragmentSubroutines.end()) {
                console::warn("unknown fragment subroutine: %s, %s", name, subroutineName);
                return;
            }

            GLint locationOfFunction = itUniform->second;
            GLuint indexFunc1 = itSubroutine->second;

            auto numActiveUniforms = static_cast<GLsizei>(fragmentSubroutineUniforms.size());

            assert(locationOfFunction < numActiveUniforms);

            std::unique_ptr<GLuint> indices(new GLuint[numActiveUniforms]);
            indices.get()[locationOfFunction] = indexFunc1;
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, numActiveUniforms, indices.get());
        }

        void Program::bindBlock(const char *blockName, GLuint point) {
            GLuint uniformBlockIndex = glGetUniformBlockIndex(handle, blockName);
            std::string n = getName();

            if (uniformBlockIndex == GL_INVALID_INDEX) {
                return;
            }

            glUniformBlockBinding(handle, uniformBlockIndex, point);

            GLint active_uniforms = 0;
            glGetActiveUniformBlockiv(handle, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &active_uniforms);

            std::vector<GLuint> uniform_indices(active_uniforms, 0);
            glGetActiveUniformBlockiv(handle, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, reinterpret_cast<GLint*>(&uniform_indices[0]));

            std::vector<GLint> name_lengths(uniform_indices.size(), 0);
            glGetActiveUniformsiv(handle, uniform_indices.size(), &uniform_indices[0], GL_UNIFORM_NAME_LENGTH, &name_lengths[0]);
        }

        GLint Program::getUniformLoc(const char *locationName) const {
            return glGetUniformLocation(handle, locationName);
        }

        GLint Program::getUniformCacheLoc(const std::string &locationName) const {
            auto it = uniformIndexCache.find(locationName);

            if (it != uniformIndexCache.end()) {
                return it->second;
            } else {
                GLint loc = glGetUniformLocation(handle, locationName.c_str());

                return loc;
            }
        }

        GLint Program::getUniformCacheLoc(const UniformName &uniformName) const {
            auto it = uniformIndexCache2.find(uniformName);

            if (it != uniformIndexCache2.end()) {
                return it->second;
            } else {
                GLint loc = glGetUniformLocation(handle, uniformName.name);

                return loc;
            }
        };

        template<typename T>
        void Program::set(const std::string &name, const T &ref) {
            gl::uniform<T>(getUniformCacheLoc(name), 1, ref);
        }

        void Program::setInt(const std::string &name, const int &value) const {
            glUniform1i(getUniformCacheLoc(name), value);
        }

        void Program::setFloat(const std::string &name, const float &value) const {
            glUniform1f(getUniformCacheLoc(name), value);
        }

        /* Mat 2x2 */
        void Program::setMat(const std::string &name, const glm::mat2 &mat) const {
            glUniformMatrix2fv(getUniformCacheLoc(name), 1, GL_FALSE, &mat[0][0]);
        }

        /* Mat 3x3 */
        void Program::setMat(const std::string &name, const glm::mat3 &mat) const {
            glUniformMatrix3fv(getUniformCacheLoc(name), 1, GL_FALSE, &mat[0][0]);
        }

        /* Mat 4x4 */

        void Program::setMat(const std::string &name, const glm::mat4 &mat) const {
            glUniformMatrix4fv(getUniformCacheLoc(name), 1, GL_FALSE, &mat[0][0]);
        }

        /* Mat 4x4 vector */
        void Program::setMat(const std::string &name, const std::vector<glm::mat4> &mats) const {
            glUniformMatrix4fv(getUniformCacheLoc(name), mats.size(), GL_FALSE, &mats.front()[0][0]);
        }

        void Program::setMat(const std::string &name, const int size, const glm::mat4 *mats) const {
            glUniformMatrix4fv(getUniformCacheLoc(name), size, GL_FALSE, &mats[0][0][0]);
        }

        void Program::setMat(const std::string &name, const int size, const float *numbers) const {
            glUniformMatrix4fv(getUniformCacheLoc(name.c_str()), size, GL_FALSE, numbers);
        }

        /* Vec2 */
        void Program::setVec(const std::string &name, const glm::vec2 &vec) const {
            glUniform3fv(getUniformCacheLoc(name), 1, glm::value_ptr(vec));
        }

        /* Vec3 */
        void Program::setVec(const std::string &name, const glm::vec3 &vec) const {
            glUniform3fv(getUniformCacheLoc(name), 1, glm::value_ptr(vec));
        }

        /* Vec4 */
        void Program::setVec(const std::string &name, const glm::vec4 &vec) const {
            glUniform3fv(getUniformCacheLoc(name), 1, glm::value_ptr(vec));
        }

        const std::string Program::getName() {
            return name;
        }

        void Program::collectUniforms() {
            GLint i = 0;
            GLint count = 0;

            GLint size = 0;
            GLenum type;

            const GLsizei bufSize = 128;
            GLchar name[bufSize];
            GLsizei length;
            std::string programName = getName();

            glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count);

            uniformIndexCache.reserve(static_cast<unsigned long int>(count));

            for (i = 0; i < count; i++)
            {
                glGetActiveUniform(handle, (GLuint)i, bufSize, &length, &size, &type, name);

                GLint index = glGetUniformLocation(handle, name);

//                if (index == -1) {
//                    return; // named uniform block or optimized variable
//                }

                uniformIndexCache.insert({name, index});
                uniformIndexCache2.insert({UniformName(name), index});
            }
        }

        void Program::collectSubroutines() {
            collectSubroutinesInformation(GL_VERTEX_SHADER, &vertexSubroutineUniforms, &vertexSubroutines);
            collectSubroutinesInformation(GL_FRAGMENT_SHADER, &fragmentSubroutineUniforms, &fragmentSubroutines);
            collectSubroutinesInformation(GL_GEOMETRY_SHADER, &geometrySubroutineUniforms, &geometrySubroutines);
        }

        void Program::collectSubroutinesInformation(
                GLenum shaderType,
                ProgramUniformSubroutines *uniforms,
                ProgramSubroutines *subroutines
        ) {
            GLsizei numActiveUniforms = 0;
            uint bufferSize = 128;

            glGetProgramStageiv(handle, shaderType, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &numActiveUniforms);

            for (GLuint i = 0; i < numActiveUniforms; i++) {
                GLchar uniformName[bufferSize];
                GLsizei length;

                glGetActiveSubroutineUniformName(handle, shaderType, i, bufferSize, &length, uniformName);

                uniforms->insert({ uniformName, i });
            }

            GLsizei length = 0;
            GLchar subroutineName[bufferSize];
            GLuint i = 0;

            GLint numActiveSubroutines = 0;
            glGetProgramStageiv(handle, shaderType, GL_ACTIVE_SUBROUTINES, &numActiveSubroutines);
            if (numActiveSubroutines == 0) {
                return;
            }

            do {
                length = 0;
                subroutineName[0] = '\0';

                glGetActiveSubroutineName(handle, shaderType, i, bufferSize, &length, subroutineName);

                if (length > 0) {
                    GLuint subroutineIndex = glGetSubroutineIndex(handle, shaderType, subroutineName);

                    subroutines->insert({ subroutineName, subroutineIndex });
                }

                i++;
            } while (i < numActiveSubroutines);
        }

        void Program::enableFragmentSubroutines(const std::vector<std::pair<std::string, std::string>>& subroutines) {
            auto numActiveUniforms = static_cast<GLsizei>(fragmentSubroutineUniforms.size());

            assert(locationOfFunction < numActiveUniforms);
            assert(subroutines.size() == numActiveUniforms);

            std::unique_ptr<GLuint> indices(new GLuint[numActiveUniforms]);

            for (const auto& pair : subroutines) {
                auto itUniform = fragmentSubroutineUniforms.find(pair.first);
                auto itSubroutine = fragmentSubroutines.find(pair.second);

                if (itUniform == fragmentSubroutineUniforms.end()) {
                    console::warn("unknown fragment uniform subroutine: %s, %s", name, pair.first);
                    return;
                }

                if (itSubroutine == fragmentSubroutines.end()) {
                    console::warn("unknown fragment subroutine: %s, %s", name, pair.second);
                    return;
                }

                GLint locationOfFunction = itUniform->second;
                GLuint indexFunc1 = itSubroutine->second;
                indices.get()[locationOfFunction] = indexFunc1;
            }

            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, numActiveUniforms, indices.get());
            OpenglCheckErrors();
        }
    }
}
