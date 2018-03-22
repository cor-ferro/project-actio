#include "shader.h"

Shader::Shader(ShaderType type)
	: success(false)
	, handle(-1)
	, type(type)
{
	create();
}

void Shader::create() {
	if (handle != -1) {
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

void Shader::reloadSources()
{
	loadFromFile(this->filePath);
}

void Shader::loadFromFile(std::string filePath)
{
	this->filePath = filePath;
	std::string shaderStr = loadFile(filePath.c_str());

	const char * shaderSource = shaderStr.c_str();
	setSource(shaderSource);
}

void Shader::setSource(const char * source)
{
	glShaderSource(handle, 1, &source, NULL);
}

bool Shader::compile()
{
	GLint shaderStatusSuccess;
	GLchar infoLog[512];

	glCompileShader(handle);
	glGetShaderiv(handle, GL_COMPILE_STATUS, &shaderStatusSuccess);

	if (!shaderStatusSuccess) {
		glGetShaderInfoLog(handle, 512, NULL, infoLog);
		console::err("Shader error:");
		console::err(infoLog);
		success = false;
	} else {
		success = true;
	}

	return success;
}

void Shader::freeSources()
{
	if (handle >= 0) {
		glDeleteShader(handle);
		handle = -1;
	}
}

bool Shader::Exists(std::string filePath)
{
	std::ifstream f(filePath.c_str());
    return f.good();
}
