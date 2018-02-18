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
#include "../../app/app.h"

#include <sys/inotify.h>

namespace Opengl {

struct Program {
	enum Program_Flags {
		Watch_Changes = 0x1
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
	void init(std::string, uint flags = 0x0);
	void initShader();
	void use();
	void nouse();

	GLint getUniformLoc(const char *) const;
	GLint getUniformCacheLoc(std::string locationName) const;
	GLint getUniformCacheLoc(Opengl::Uniform::Common& uniform) const;

	GLuint getSubroutineIndex(unsigned int& shaderType, std::string soubroutineName);
	GLuint getSubroutineCacheIndex(unsigned int& shaderType, std::string soubroutineName);

	void setFloat(const std::string&, const float&) const;
	void setInt(const std::string&, const int&) const;
	void setMat(const std::string&, const mat4&) const;
	void setMat(const std::string&, const mat3&) const;
	void setMat(const std::string&, const mat2&) const;

	void setMat(const std::string&, const std::vector<mat4> * mats) const;
	void setMat(const std::string&, const int size, const mat4 * mats) const;
	void setMat(const std::string&, const int size, const float * numbers) const;

	void setVec(const std::string&, const vec2 &) const;
	void setVec(const std::string&, const vec3 &) const;
	void setVec(const std::string&, const vec4 &) const;

	void setMat(Opengl::Uniform::Common, const mat4&) const;
	void setMat(Opengl::Uniform::Common, const std::vector<mat4>*) const;
	void setFloat(Opengl::Uniform::Common, const float&) const;
	void setVec(Opengl::Uniform::Common, const glm::vec2&) const;
	void setVec(Opengl::Uniform::Common, const glm::vec3&) const;
	void setVec(Opengl::Uniform::Common, const glm::vec4&) const;

	void enableSubroutine(unsigned int shaderType, std::string& subroutinName);
	void enableVertexSubroutine(std::string subroutinName);
	void enableFragmentSubroutine(std::string subroutinName);

	void bindBlock(const char *blockName, int point);

	void initUniformCache(std::vector<std::string> locations);
	void initUniformCache(std::map <Opengl::Uniform::Common, std::string> locations);

	void checkShadersUpdate();

	const GLuint getHandle() const;

	GLuint handle;

	Shader * vertexShader;
	Shader * fragmentShader;
	Shader * geometryShader;

	std::string name;
private:
	void setupWatch();
	void removeWatch();

	bool compile();

	WatchDescriptor * watcher;
	bool isUsed;
	bool success;
	std::string getShaderPath(std::string);
	std::unordered_map<std::string, GLint> uniformIndexCache;
	std::map<short int, GLint> uniformIndexCache2;
	std::unordered_map<std::string, GLuint> subroutineVertexIndexCache;
	std::unordered_map<std::string, GLuint> subroutineFragmentIndexCache;
};

}

#endif /* PROGRAM_H_ */
