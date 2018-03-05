#version 430 core

layout (location = 0) out vec3 gWorldPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in VertexData {
	vec4 worldPosition;
	vec3 worldNormal;
	vec3 texCoord;
} inData;

uniform samplerCube cubeTexture;

void main()
{
    gWorldPosition = inData.worldPosition.xyz;
    gNormal = inData.worldNormal;
	gAlbedo = vec4(texture(cubeTexture, inData.texCoord).xyz, 0.0);
}
