#include "shader.h"

Shader::Shader(Shader_Type type)
{
	success = false;
	switch (type) {
		case VERTEX:
			handle = glCreateShader(GL_VERTEX_SHADER);
			break;
		case FRAGMENT:
			handle = glCreateShader(GL_FRAGMENT_SHADER);
			break;
	}
}

void Shader::loadFromFile(std::string filePath)
{
	std::string shaderStr = loadFile(filePath.c_str());

	const char * shaderSource = shaderStr.c_str();
	setSource(shaderSource);
}

void Shader::setSource(const char * source)
{
	glShaderSource(handle, 1, &source, NULL);
}

void Shader::compile()
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
}

void Shader::freeSources()
{
	glDeleteShader(handle);
}
