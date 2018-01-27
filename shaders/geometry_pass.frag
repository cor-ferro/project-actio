#version 430 core

layout (location = 0) out vec3 gWorldPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in VertexData {
	vec3 position;
	vec3 worldPosition;
	vec2 texCoord;
	vec3 normal;
} inData;

in mat3 TBN;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D heightTexture;

void main()
{
	vec3 norm = normalize(inData.normal);
	norm = texture(heightTexture, inData.texCoord).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	norm = normalize(TBN * norm);

	gWorldPosition = inData.worldPosition;
	gNormal = norm;
	gAlbedo = vec4(
		texture(diffuseTexture, inData.texCoord).xyz,
		texture(specularTexture, inData.texCoord).x
	);
}