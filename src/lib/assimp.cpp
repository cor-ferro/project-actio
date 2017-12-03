#include "assimp.h"

namespace libAi {
	mat4 toNativeType(aiMatrix4x4& aiMat)
	{
		float values[16] = {
			aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
			aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
			aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
			aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
		};

		return glm::make_mat4(values);
	}

	vec3 toNativeType(aiVector3D& aiVec)
	{
		return vec3(aiVec.x, aiVec.y, aiVec.z);
	}
}