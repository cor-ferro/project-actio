#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <GL/glew.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "program.h"
#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include "../../lib/console.h"
#include "../../lib/types.h"
#include "../../app/app.h"

namespace Opengl {

struct Program {
	Program();
	explicit Program(std::string);
	~Program();
	void init(std::string);
	void use();
	void nouse();

	unsigned int getUniformLoc(const char *) const;
	unsigned int getUniformCacheLoc(std::string locationName) const;

	void setFloat(const std::string &, const float &) const;
	void setInt(const std::string &, const int &) const;
	void setMat(const std::string &, const mat4 &) const;
	void setMat(const std::string &, const mat3 &) const;
	void setMat(const std::string &, const mat2 &) const;

	void setMat(const std::string &, const std::vector<mat4>*) const;

	void setVec(const std::string &, const vec2 &) const;
	void setVec(const std::string &, const vec3 &) const;
	void setVec(const std::string &, const vec4 &) const;

	void bindBlock(const char *blockName, int point);

	void initUniformCache(std::vector<std::string> locations);

	GLuint handle;
	std::vector<Shader*> shaders;

	Shader * vertexShader;
	Shader * fragmentShader;

	std::string name;
private:
	bool isUsed;
	bool success;
	std::string getShaderPath(std::string);
	std::unordered_map<std::string, unsigned int> uniformIndexCache;
};

}

#endif /* PROGRAM_H_ */
