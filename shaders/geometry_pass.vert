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

uniform mat4 boneTransforms[MAX_BONES];
uniform mat4 boneOffsets[MAX_BONES];

subroutine mat4 BoneTransform();

subroutine (BoneTransform) mat4 BoneTransformEnabled() {
	mat4 boneTransform = (boneTransforms[BoneIDs[0]] * boneOffsets[BoneIDs[0]]) * Weights[0];
	boneTransform+= (boneTransforms[BoneIDs[1]] * boneOffsets[BoneIDs[1]]) * Weights[1];
	boneTransform+= (boneTransforms[BoneIDs[2]] * boneOffsets[BoneIDs[2]]) * Weights[2];
	boneTransform+= (boneTransforms[BoneIDs[3]] * boneOffsets[BoneIDs[3]]) * Weights[3];

	return boneTransform;
}

subroutine (BoneTransform) mat4 BoneTransformDisabled() {
	return mat4(1.0);
}

subroutine uniform BoneTransform getBoneTransform;

out VertexData {
	vec3 position;
	vec3 worldPosition;
	vec2 texCoord;
	vec3 normal;
} outData;

uniform mat4 model;

void main()
{
	mat4 boneTransform = getBoneTransform();
	vec4 worldPos = model * boneTransform * vec4(aPos, 1.0);
	vec3 worldNormal = mat3(transpose(inverse(model))) * (vec4(aNormal, 1.0)).xyz;

	outData.position = aPos;
	outData.worldPosition = vec3(worldPos);
	outData.normal = normalize(worldNormal);
	outData.texCoord = aTexCoord;

	gl_Position = projection * view * worldPos;
}