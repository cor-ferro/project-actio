#version 430 core
#define MAX_BONES 100

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 Weights;

layout (std140, binding = 0)
uniform Matrices {
	mat4 projection;
	mat4 view;
};

out vec4 vertexColor;
out vec3 fragmentPosition;
out vec3 normal;
out vec3 normal1;
out vec2 texCoord;
out mat3 TBN;

out vec3 TangentViewPos;
out vec3 TangentLightDir;
out vec3 TangentFragPos;

uniform vec3 viewPos;

uniform mat4 mvp;
uniform mat4 model;

uniform float time;

uniform mat4 bones[MAX_BONES];

subroutine mat4 BoneTransform();

subroutine (BoneTransform) mat4 BoneTransformEnabled() {
	mat4 boneTransform = bones[BoneIDs[0]] * Weights[0];
	boneTransform+= bones[BoneIDs[1]] * Weights[1];
	boneTransform+= bones[BoneIDs[2]] * Weights[2];
	boneTransform+= bones[BoneIDs[3]] * Weights[3];

	return boneTransform;
}

subroutine (BoneTransform) mat4 BoneTransformDisabled() {
	return mat4(1.0);
}

subroutine uniform BoneTransform getBoneTransform;

void main()
{
	vec3 pos = aPos;
	mat4 boneTransform = getBoneTransform();

	fragmentPosition = vec3(model * boneTransform * vec4(aPos, 1.0));
	normal = mat3(transpose(inverse(model))) * (boneTransform * vec4(aNormal, 1.0)).xyz; // @todo: пустое преобразование ломает нормали
	normal1 = aNormal;
	
	texCoord = aTexCoord;
	
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	TBN = transpose(mat3(T, B, N));

	TangentViewPos = TBN * viewPos;
	TangentFragPos = TBN * fragmentPosition;

	gl_Position = projection * view * vec4(fragmentPosition, 1.0);
}