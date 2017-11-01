#ifndef SHADER_H_
#define SHADER_H_

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../../lib/console.h"

enum Shader_Type {
	VERTEX = 1,
	FRAGMENT = 2
};

struct Shader {
	Shader(Shader_Type type);
	void loadFromFile(std::string file_path);
	void setSource(const char * source);
	void compile();
	void freeSources();

	GLuint handle;
	bool success;
};

static std::string loadFile(const char * file_path) {
	std::string content;
	std::ifstream fileStream(file_path, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "Could not open file " << file_path << std::endl;
		return "";
	}

	std::string line = "";

	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();

	return content;
}

#endif
