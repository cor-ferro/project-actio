#include "program.h"

namespace Opengl {
	Program::Program()
		: handle(-1)
		, success(false)
		, isUsed(false)
		, fragmentShader(nullptr)
		, vertexShader(nullptr)
		, geometryShader(nullptr)
		, watcher(nullptr)
	{}

	Program::Program(std::string name) : Program()
	{
		init(name);
	}

	void Program::init(std::string name, uint flags)
	{
		this->name = name;
		isUsed = false;

		this->vertexShader = new Shader(Shader::VERTEX);
		this->fragmentShader = new Shader(Shader::FRAGMENT);
		this->geometryShader = nullptr;

		std::string vertexShaderPath = getShaderPath(name) + std::string(".vert");
		std::string fragmentShaderPath = getShaderPath(name) + std::string(".frag");
		std::string geometryShaderPath = getShaderPath(name) + std::string(".geom");

		this->vertexShader->loadFromFile(vertexShaderPath);
		this->vertexShader->compile();

		this->fragmentShader->loadFromFile(fragmentShaderPath);
		this->fragmentShader->compile();

		if (Shader::Exists(geometryShaderPath)) {
			this->geometryShader = new Shader(Shader::GEOMETRY);
			this->geometryShader->loadFromFile(geometryShaderPath);
			this->geometryShader->compile();
		}

		handle = glCreateProgram();

		glAttachShader(handle, this->vertexShader->handle);
		glAttachShader(handle, this->fragmentShader->handle);
		if (this->geometryShader != nullptr) {
			glAttachShader(handle, this->geometryShader->handle);
		}

		if (compile()) {
//			this->vertexShader->freeSources();
//			this->fragmentShader->freeSources();
//			if (this->geometryShader != nullptr) {
//				this->geometryShader->freeSources();
//			}
		}

		if ((flags & Watch_Changes) != 0) {
			setupWatch();
		}
	}

	bool Program::compile()
	{
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

	void Program::initShader() {

	}

	Program::~Program()
	{
		console::infop("destroy shader program %s", name.c_str());
		removeWatch();
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
		GLuint routineIndex = getSubroutineCacheIndex(shaderType, subroutineName);
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

	GLuint Program::getSubroutineIndex(unsigned int& shaderType, std::string soubroutineName)
	{
		return glGetSubroutineIndex(handle, shaderType, soubroutineName.c_str());
	}

	GLuint Program::getSubroutineCacheIndex(unsigned int& shaderType, std::string soubroutineName)
	{
		std::unordered_map<std::string, GLuint> * subroutineIndexCache = nullptr;

		if (shaderType == GL_VERTEX_SHADER) {
			subroutineIndexCache = &subroutineVertexIndexCache;
		} else if (shaderType == GL_FRAGMENT_SHADER) {
			subroutineIndexCache = &subroutineFragmentIndexCache;
		} else {
			assert(false);
		}

		auto it = subroutineIndexCache->find(soubroutineName);

		if (it == subroutineIndexCache->end()) {
			GLuint index = getSubroutineIndex(shaderType, soubroutineName);

			if (index == GL_INVALID_INDEX) {
				console::warnp("GL_INVALID_INDEX: %s", soubroutineName.c_str());
			}

			subroutineIndexCache->insert({ soubroutineName, index });
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

	void Program::checkShadersUpdate()
	{
		if (watcher == nullptr) return;

		const int EVENT_SIZE = (sizeof(struct inotify_event));
		const int EVENT_BUF_LEN = (1024 * (EVENT_SIZE + 16 ));

		char buffer[EVENT_BUF_LEN];

		int length = read(watcher->fd, buffer, EVENT_BUF_LEN);

		if (length == 0) {
			return;
		}

		int i = 0;
		while ( i < length ) {
			struct inotify_event * event = (struct inotify_event *) &buffer[i];

			glDetachShader(handle, this->vertexShader->handle);
			glDetachShader(handle, this->fragmentShader->handle);

			this->vertexShader->freeSources();
			this->fragmentShader->freeSources();

			glDeleteProgram(handle);

			handle = glCreateProgram();

			this->vertexShader->create();
			this->fragmentShader->create();

			this->vertexShader->reloadSources();
			this->fragmentShader->reloadSources();

			glAttachShader(handle, this->vertexShader->handle);
			glAttachShader(handle, this->fragmentShader->handle);

			bool isCompiledVShader = this->vertexShader->compile();
			bool isCompiledFShader = this->fragmentShader->compile();

			if (isCompiledVShader && isCompiledFShader) {
				this->compile();
				console::info("compile new program");
			}

			i += EVENT_SIZE + event->len;
		 }
	}

	void Program::setupWatch()
	{
		console::info("setup watch");
		removeWatch();

		watcher = new WatchDescriptor();
		watcher->fd = inotify_init1(IN_NONBLOCK);

		if (watcher->fd < 0) {
			console::warn("failed init inotify");
		}

		if (this->vertexShader != nullptr) {
			watcher->vertexFd = inotify_add_watch(watcher->fd, this->vertexShader->filePath.c_str(), IN_MODIFY);
		}

		if (this->fragmentShader != nullptr) {
			watcher->fragmentFd = inotify_add_watch(watcher->fd, this->fragmentShader->filePath.c_str(), IN_MODIFY);
		}

		if (this->geometryShader != nullptr) {
			watcher->geometryFd = inotify_add_watch(watcher->fd, this->geometryShader->filePath.c_str(), IN_MODIFY);
		}

	}

	void Program::removeWatch()
	{
		if (watcher != nullptr) {
			if (watcher->vertexFd >= 0) inotify_rm_watch(watcher->fd, watcher->vertexFd);
			if (watcher->fragmentFd >= 0) inotify_rm_watch(watcher->fd, watcher->fragmentFd);
			if (watcher->geometryFd >= 0) inotify_rm_watch(watcher->fd, watcher->geometryFd);
			close(watcher->fd);
			delete watcher;
			watcher = nullptr;
		}
	}
}
