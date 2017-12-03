#include "program.h"

namespace Opengl {
	Program::Program()
		: handle(-1)
		, success(false)
		, isUsed(false)
		, fragmentShader(nullptr)
		, vertexShader(nullptr)
	{}

	Program::Program(std::string name)
	{
		init(name);
	}

	void Program::init(std::string name)
	{
		this->name = name;
		isUsed = false;

		Shader * newVertexShader = new Shader(Shader::VERTEX);
		Shader * newFragmentShader = new Shader(Shader::FRAGMENT);
		Shader * newGeometryShader = nullptr;

		std::string vertexShaderPath = getShaderPath(name) + std::string(".vert");
		std::string fragmentShaderPath = getShaderPath(name) + std::string(".frag");
		std::string geometryShaderPath = getShaderPath(name) + std::string(".geom");

		newVertexShader->loadFromFile(vertexShaderPath);
		newVertexShader->compile();

		newFragmentShader->loadFromFile(fragmentShaderPath);
		newFragmentShader->compile();

		if (Shader::Exists(geometryShaderPath)) {
			newGeometryShader = new Shader(Shader::GEOMETRY);
			newGeometryShader->loadFromFile(geometryShaderPath);
			newGeometryShader->compile();
		}

		int success;
		char infoLog[4096];

		handle = glCreateProgram();
		glAttachShader(handle, newVertexShader->handle);
		glAttachShader(handle, newFragmentShader->handle);
		if (newGeometryShader != nullptr) {
			glAttachShader(handle, newGeometryShader->handle);
		}

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

			this->vertexShader = newVertexShader;
			this->fragmentShader = newFragmentShader;
			this->geometryShader = newGeometryShader;

			this->vertexShader->freeSources();
			this->fragmentShader->freeSources();
			if (newGeometryShader != nullptr) {
				this->geometryShader->freeSources();
			}
		}
	}

	Program::~Program()
	{
		console::info("destroy shader program");
	}

	const GLuint Program::getHandle() const
	{
		return handle;
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
			GLint index = getUniformLoc(locationName.c_str());

			if (index == -1) {
				console::warn("uniform ", locationName, " cache failed.");
			}

			uniformIndexCache.insert({locationName, index});
		}
	}

	void Program::initUniformCache(std::map <Opengl::Uniform::Common, std::string> locations)
	{
		for (auto it = locations.begin(); it != locations.end(); it++)
		{
			std::string& locationName = it->second;

			GLint index = getUniformLoc(locationName.c_str());

			if (index == -1) {
				console::warn("uniform ", locationName, " cache2 failed.");
			} else {
				uniformIndexCache2.insert({ it->first, index });
			}
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

	void Program::bindBlock(const char * blockName, int point)
	{
		GLuint uniformBlockIndex = glGetUniformBlockIndex(handle, blockName);

		if (uniformBlockIndex == GL_INVALID_INDEX)
		{
			console::warn("failed bind block", blockName, "at point", point);
		}

		glUniformBlockBinding(handle, uniformBlockIndex, point);
	}

	GLint Program::getUniformLoc(const char * locationName) const
	{
		return glGetUniformLocation(handle, locationName);
	}

	GLint Program::getUniformCacheLoc(std::string locationName) const
	{
		std::unordered_map<std::string, GLint>::const_iterator got = uniformIndexCache.find(locationName);

		#ifndef OPENGL_PROGRAM_UNIFORM_CACHE
		if (got == uniformIndexCache.end()) {
			return getUniformLoc(locationName.c_str());
		} else {
			return got->second;
		}
		#endif

		#ifdef OPENGL_PROGRAM_UNIFORM_CACHE
		return got->second;
		#endif
	}

	GLint Program::getUniformCacheLoc(Opengl::Uniform::Common& uniform) const
	{
		return uniformIndexCache2.find(uniform)->second;
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

	void Program::setFloat(Opengl::Uniform::Common uniform, const float &value) const
	{
		glUniform1f(getUniformCacheLoc(uniform), value);
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
	void Program::setMat(Opengl::Uniform::Common uniform, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(getUniformCacheLoc(uniform), 1, GL_FALSE, &mat[0][0]);
	}

	void Program::setMat(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(getUniformCacheLoc(name), 1, GL_FALSE, &mat[0][0]);
	}

	/* Mat 4x4 vector */
	void Program::setMat(Opengl::Uniform::Common uniform, const std::vector<glm::mat4> * mats) const
	{
		glUniformMatrix4fv(getUniformCacheLoc(uniform), mats->size(), GL_FALSE, &mats->front()[0][0]);
	}

	void Program::setMat(const std::string &name, const std::vector<glm::mat4> * mats) const
	{
		glUniformMatrix4fv(getUniformCacheLoc(name), mats->size(), GL_FALSE, &mats->front()[0][0]);
	}

	/* Vec2 */
	void Program::setVec(const std::string &name, const glm::vec2 &vec) const
	{
		glUniform3fv(getUniformCacheLoc(name), 1, glm::value_ptr(vec));
	}

	void Program::setVec(Opengl::Uniform::Common uniform, const glm::vec2 &vec) const
	{
		glUniform3fv(getUniformCacheLoc(uniform), 1, glm::value_ptr(vec));
	}

	/* Vec3 */
	void Program::setVec(const std::string &name, const glm::vec3 &vec) const
	{
		glUniform3fv(getUniformCacheLoc(name), 1, glm::value_ptr(vec));
	}

	void Program::setVec(Opengl::Uniform::Common uniform, const glm::vec3 &vec) const
	{
		glUniform3fv(getUniformCacheLoc(uniform), 1, glm::value_ptr(vec));
	}

	/* Vec4 */
	void Program::setVec(const std::string &name, const glm::vec4 &vec) const
	{
		glUniform3fv(getUniformCacheLoc(name), 1, glm::value_ptr(vec));
	}

	void Program::setVec(Opengl::Uniform::Common uniform, const glm::vec4 &vec) const
	{
		glUniform3fv(getUniformCacheLoc(uniform), 1, glm::value_ptr(vec));
	}
}
