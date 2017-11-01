#version 430 core

layout (location = 0) out vec3 gWorldPosition;
layout (location = 1) out vec3 gDiffuse;
layout (location = 2) out vec3 gNormal;
layout (location = 3) out vec3 gTexCoord;

in VertexData {
	vec3 position;
	vec3 worldPosition;
	vec2 texCoord;
	vec3 normal;
} inData;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	gWorldPosition = inData.worldPosition;
	gDiffuse = texture(texture_diffuse1, inData.texCoord).xyz;
	gNormal = normalize(inData.normal);
	gTexCoord = vec3(inData.texCoord, 0.0);
}