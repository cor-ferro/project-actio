#ifndef LIB_ASSIMP_H_
#define LIB_ASSIMP_H_

#include <assimp/matrix4x4.h>
#include <glm/gtc/type_ptr.hpp>
#include "./types.h"

mat4 aiMatToMat(aiMatrix4x4& aiMat);

#endif /* LIB_ASSIMP_H_ */
