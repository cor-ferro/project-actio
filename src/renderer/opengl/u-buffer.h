/*
 * u-buffer.h
 *
 *  Created on: Oct 6, 2017
 *      Author: demitriy
 */

#ifndef U_BUFFER_H_
#define U_BUFFER_H_

#include <GL/glew.h>
#include <cstddef>
#include <vector>
#include <unordered_map>
#include <utility>
#include "../../lib/console.h"

typedef std::unordered_map<short int, std::pair<std::size_t, std::size_t>> UBUfferOffset;

namespace Renderer {

struct UBuffer {
	UBuffer();
	~UBuffer();

	void init(std::vector<std::size_t> sizes, GLuint mountPoint);
	void set(short int index, const GLvoid * data);
	void use();
	void nouse();

private:
	GLuint ubo_;
	unsigned int dataSize_;
	bool isUsed_;
	UBUfferOffset offsets_;
};

}


#endif /* U_BUFFER_H_ */
