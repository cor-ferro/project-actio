#include "program.h"

namespace Opengl {

Program::Program()
{
	handle = -1;
	success = false;
	isUsed = false;
	shaders = std::vector<Shader*>();
	this->fragmentShader = 0;
	this->vertexShader = 0;
}

Program::Program(std::string name)
{
	init(name);
}

void Program::init(std::string name)
{
	this->name = name;
	isUsed = false;
	shaders = std::vector<Shader*>();
	shaders.reserve(2);

	Shader * newVertexShader = new Shader(VERTEX);
	Shader * newFragmentShader = new Shader(FRAGMENT);

	newVertexShader->loadFromFile(getShaderPath(name) + std::string(".vert"));
	newVertexShader->compile();

	newFragmentShader->loadFromFile(getShaderPath(name) + std::string(".frag"));
	newFragmentShader->compile();

	int success;
	char infoLog[4096];

	handle = glCreateProgram();
	glAttachShader(handle, newVertexShader->handle);
	glAttachShader(handle, newFragmentShader->handle);
	glLinkProgram(handle);
	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(handle, 4096, NULL, infoLog);
		std::cout
			<< "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl
			<< infoLog
			<< std::endl;
		success = false;
	} else {
		success = true;

		if (newVertexShader->success) shaders.push_back(newVertexShader);
		if (newFragmentShader->success) shaders.push_back(newFragmentShader);

		this->vertexShader = newVertexShader;
		this->fragmentShader = newFragmentShader;

		this->vertexShader->freeSources();
		this->fragmentShader->freeSources();
	}
}

Program::~Program()
{
	console::info("destroy shader program");
}

std::string Program::getShaderPath(std::string name)
{
	return App::instance().shadersPath("/" + name);
}

void Program::use()
{
	glUseProgram(handle);
	isUsed = true;
}

void Program::nouse()
{
	if (isUsed == true) {
		glUseProgram(0);
		isUsed = false;
	} else {
		console::warn("try no use shader failed");
	}
}

void Program::initUniformCache(std::vector<std::string> locations)
{
	uniformIndexCache.reserve(locations.size());

	for (std::string locationName : locations)
	{
		unsigned int index = getUniformLoc(locationName.c_str());

		if (index == -1) {
			console::warn("uniform ", locationName, " cache failed.");
		}

		uniformIndexCache.insert({locationName, index});
	}
}

void Program::enableSubroutine(unsigned int shaderType, std::string& subroutineName)
{
	unsigned int routineIndex = getSubroutineCacheIndex(shaderType, subroutineName);
	glUniformSubroutinesuiv(shaderType, 1, &routineIndex);
}

void Program::enableVertexSubroutine(std::string& subroutineName)
{
	enableSubroutine(GL_VERTEX_SHADER, subroutineName);
}

void Program::enableFragmentSubroutine(std::string& subroutineName)
{
	enableSubroutine(GL_FRAGMENT_SHADER, subroutineName);
}

void Program::bindBlock(const char *blockName, int point)
{
	GLuint uniformBlockIndex = glGetUniformBlockIndex(handle, blockName);

	if (uniformBlockIndex == GL_INVALID_INDEX)
	{
		console::warn("failed bind block", blockName, "at point", point);
	}

	glUniformBlockBinding(handle, uniformBlockIndex, point);
}

unsigned int Program::getUniformLoc(const char * locationName) const
{
	return glGetUniformLocation(handle, locationName);
}

unsigned int Program::getUniformCacheLoc(std::string locationName) const
{
	std::unordered_map<std::string, unsigned int>::const_iterator got = uniformIndexCache.find(locationName);

	#ifndef OPENGL_PROGRAM_UNIFORM_CACHE
	if (got == uniformIndexCache.end()) {
		// console::warn("Cache uniform not found. Program:", this->name, " location:", locationName);

		// unsigned int loc = getUniformLoc(locationName.c_str());
		// if (loc == -1 && this->name == "forward")
		// 	console::warn("Uniform not found. Program: ", this->name, ", location: ", locationName);
		
		// return loc;

		return getUniformLoc(locationName.c_str());
	} else {
		return got->second;
	}
	#endif

	#ifdef OPENGL_PROGRAM_UNIFORM_CACHE
	return got->second;
	#endif
}

unsigned int Program::getSubroutineIndex(unsigned int& shaderType, std::string soubroutineName)
{
	return glGetSubroutineIndex(handle, shaderType, soubroutineName.c_str());
}

unsigned int Program::getSubroutineCacheIndex(unsigned int& shaderType, std::string soubroutineName)
{
	std::unordered_map<std::string, unsigned int> * subroutineIndexCache = nullptr;

	if (shaderType == GL_VERTEX_SHADER) {
		subroutineIndexCache = &subroutineVertexIndexCache;
	} else if (shaderType == GL_FRAGMENT_SHADER) {
		subroutineIndexCache = &subroutineFragmentIndexCache;
	} else {
		assert(false);
	}

	auto it = subroutineIndexCache->find(soubroutineName);

	if (it == subroutineIndexCache->end()) {
		unsigned int index = getSubroutineIndex(shaderType, soubroutineName);
		subroutineIndexCache->insert(std::pair<std::string, unsigned int>(soubroutineName, index));
		return -1;
	}

	return it->second;
}

void Program::setInt(const std::string &name, const int &value) const
{
	glUniform1i(getUniformCacheLoc(name), value);
}

void Program::setFloat(const std::string &name, const float &value) const
{
	glUniform1f(getUniformCacheLoc(name), value);
}

/* Mat 2x2 */
void Program::setMat(const std::string &name, const glm::mat2 &mat) const
{
	glUniformMatrix2fv(getUniformCacheLoc(name), 1, GL_FALSE, &mat[0][0]);
}

/* Mat 3x3 */
void Program::setMat(const std::string &name, const glm::mat3 &mat) const
{
	glUniformMatrix3fv(getUniformCacheLoc(name), 1, GL_FALSE, &mat[0][0]);
}

/* Mat 4x4 */
void Program::setMat(const std::string &name, const glm::mat4 &mat) const
{
	glUniformMatrix4fv(getUniformCacheLoc(name), 1, GL_FALSE, &mat[0][0]);
}

/* Mat 4x4 vector */
void Program::setMat(const std::string &name, const std::vector<glm::mat4> * mats) const
{
	glUniformMatrix4fv(getUniformCacheLoc(name), mats->size(), GL_FALSE, &mats->front()[0][0]);
}

/* Vec2 */
void Program::setVec(const std::string &name, const glm::vec2 &vec) const
{
	glUniform3fv(getUniformCacheLoc(name), 1, glm::value_ptr(vec));
}

/* Vec3 */
void Program::setVec(const std::string &name, const glm::vec3 &vec) const
{
	glUniform3fv(getUniformCacheLoc(name), 1, glm::value_ptr(vec));
}

/* Vec4 */
void Program::setVec(const std::string &name, const glm::vec4 &vec) const
{
	glUniform3fv(getUniformCacheLoc(name), 1, glm::value_ptr(vec));
}

}
