#ifndef LIB_ASSIMP_H_
#define LIB_ASSIMP_H_

#include <algorithm>
#include <iomanip>
#include <math.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <glm/gtc/type_ptr.hpp>
#include "./types.h"
#include "./console.h"

namespace libAi {
    mat4 toNativeType(const aiMatrix4x4& aiMat);
    vec3 toNativeType(const aiVector3D& aiVec);
    quat toNativeType(const aiQuaternion& aiQuat);
    
    void print(const aiMatrix4x4& aiMat);
    void print(const aiVector3D& aiVec);
    void print(const aiQuaternion& aiQuat);
}

#endif /* LIB_ASSIMP_H_ */
