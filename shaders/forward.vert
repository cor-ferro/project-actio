#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec4 BoneIDs;
layout (location = 6) in vec4 Weights;

layout (std140, binding = 0)
uniform Matrices {
	mat4 projection;
	mat4 view;
};

out vec4 vertexPosition;
out vec4 vertexColor;
out vec3 fragmentPosition;
out vec3 normal;
out vec3 normal1;
out vec2 texCoord;
out mat3 TBN;

uniform mat4 mvp;
uniform mat4 model;

uniform float time;

void main()
{
	vec3 pos = aPos;

	vertexPosition = vec4(pos, 1.0);
	fragmentPosition = vec3(model * vec4(aPos, 1.0));
	normal = mat3(transpose(inverse(model))) * aNormal;
	normal1 = aNormal;
	
	texCoord = aTexCoord;
	
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	TBN = mat3(T, B, N);

	gl_Position = projection * view * vec4(fragmentPosition, 1.0);
}