#version 430 core
in vec3 texCoord;
out vec4 FragColor;

uniform samplerCube cubeTexture;

void main()
{
    FragColor = texture(cubeTexture, texCoord);
}