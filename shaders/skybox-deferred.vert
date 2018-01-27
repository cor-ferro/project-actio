#version 430 core
layout (location = 0) in vec3 aPos;

layout (std140, binding = 0)
uniform Matrices {
	mat4 projection;
	mat4 view;
};

out VertexData {
	vec4 worldPosition;
	vec3 worldNormal;
	vec3 texCoord;
} outData;

void main()
{
    vec4 position = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
    
    outData.worldPosition = position;
    outData.worldNormal = vec3(0.0, 1.0, 0.0);
    outData.texCoord = -aPos;

    gl_Position = position.xyww;
}