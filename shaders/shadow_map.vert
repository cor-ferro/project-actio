#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 gWVP;
uniform mat4 model;

out VSOutput
{
    vec3 WorldPos;
} outData;

void main()
{
    gl_Position = gWVP * model * vec4(aPos, 1.0);
    outData.WorldPos = gl_Position.xyz;
}