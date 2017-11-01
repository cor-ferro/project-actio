#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

//layout (std140) uniform Matrices {
//	mat4 projection;
//	mat4 view;
//};

out VertexData {
	vec3 position;
	vec3 worldPosition;
	vec2 texCoord;
	vec3 normal;
} outData;

uniform mat4 mvp;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	vec3 pos = aPos;
	vec4 fragmentPosition = model * vec4(pos, 1.0);

	outData.position = vec3(pos);
	outData.worldPosition = vec3(fragmentPosition.xyz);
	outData.normal = aNormal;
	outData.texCoord = aTexCoord;

	gl_Position = projection * view * vec4(fragmentPosition);
}