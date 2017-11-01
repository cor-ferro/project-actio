#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 mvp;
uniform float time;

void main()
{
	gl_Position = mvp * vec4(aPos * sin(time * 0.1), 1.0);
}