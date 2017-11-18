#include "assimp.h"

mat4 aiMatToMat(aiMatrix4x4& aiMat) {
    float values[16] = {
		aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
	};
	return glm::make_mat4(values);

	// mat4 m(0.0f);

	// m[0][0] = aiMat.a1; m[0][1] = aiMat.a2; m[0][2] = aiMat.a3; m[0][3] = aiMat.a4;
	// m[1][0] = aiMat.b1; m[1][1] = aiMat.b2; m[1][2] = aiMat.b3; m[1][3] = aiMat.b4;
	// m[2][0] = aiMat.c1; m[2][1] = aiMat.c2; m[2][2] = aiMat.c3; m[2][3] = aiMat.c4;
	// m[3][0] = aiMat.d1; m[3][1] = aiMat.d2; m[3][2] = aiMat.d3; m[3][3] = aiMat.d4;

	// return m;
}