#ifndef LIB_ASSIMP_H_
#define LIB_ASSIMP_H_

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <glm/gtc/type_ptr.hpp>
#include "./types.h"

namespace libAi {
    mat4 toNativeType(aiMatrix4x4& aiMat);
    vec3 toNativeType(aiVector3D& aiVec);
    quat toNativeType(aiQuaternion& aiQuat);
}

#endif /* LIB_ASSIMP_H_ */
