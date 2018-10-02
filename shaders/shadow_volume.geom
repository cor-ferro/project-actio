#version 430 core

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 18) out;

uniform vec3 gLightPos;
uniform mat4 gVP;
uniform mat4 gWorld;

layout (std140, binding = 0)
uniform Matrices {
	mat4 projection;
	mat4 view;
};

in VSOutput
{
  vec3 WorldPos;
} GSin[];

struct sVSOutput
{
    vec3 WorldPos;
};

float EPSILON = 0.0001;

void EmitQuad(int StartIndex, sVSOutput StartVertex, int EndIndex, sVSOutput EndVertex)
{
    vec3 LightDir = normalize(StartVertex.WorldPos - gLightPos);
    vec3 l = LightDir * EPSILON;
    gl_Position = projection * view * vec4((StartVertex.WorldPos + l), 1.0);
    EmitVertex();

    gl_Position = projection * view * vec4(LightDir, 0.0);
    EmitVertex();

    LightDir = normalize(EndVertex.WorldPos - gLightPos);
    l = LightDir * EPSILON;
    gl_Position = projection * view * vec4((EndVertex.WorldPos + l), 1.0);
    EmitVertex();

    gl_Position = projection * view * vec4(LightDir, 0.0);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec3 e1 = GSin[2].WorldPos - GSin[0].WorldPos;
    vec3 e2 = GSin[4].WorldPos - GSin[0].WorldPos;
    vec3 e3 = GSin[1].WorldPos - GSin[0].WorldPos;
    vec3 e4 = GSin[3].WorldPos - GSin[2].WorldPos;
    vec3 e5 = GSin[4].WorldPos - GSin[2].WorldPos;
    vec3 e6 = GSin[5].WorldPos - GSin[0].WorldPos;

    vec3 Normal = cross(e1,e2);
    vec3 LightDir = gLightPos - GSin[0].WorldPos;

    if (dot(Normal, LightDir) > 0.000001) {
        sVSOutput StartVertex;
        sVSOutput EndVertex;

        Normal = cross(e3,e1);

        if (dot(Normal, LightDir) <= 0) {
            StartVertex.WorldPos = GSin[0].WorldPos;
            EndVertex.WorldPos   = GSin[2].WorldPos;
            EmitQuad(0, StartVertex, 2, EndVertex);
        }

        Normal = cross(e4,e5);
        LightDir = gLightPos - GSin[2].WorldPos;

        if (dot(Normal, LightDir) <= 0) {
            StartVertex.WorldPos = GSin[2].WorldPos;
            EndVertex.WorldPos   = GSin[4].WorldPos;
            EmitQuad(2, StartVertex, 4, EndVertex);
        }

        Normal = cross(e2,e6);
        LightDir = gLightPos - GSin[4].WorldPos;

        if (dot(Normal, LightDir) <= 0) {
            StartVertex.WorldPos = GSin[4].WorldPos;
            EndVertex.WorldPos   = GSin[0].WorldPos;
            EmitQuad(4, StartVertex, 0, EndVertex);
        }

        vec3 LightDir = (normalize(GSin[0].WorldPos - gLightPos)) * EPSILON;
        gl_Position = projection * view * vec4((GSin[0].WorldPos + LightDir), 1.0);
        EmitVertex();

        LightDir = (normalize(GSin[2].WorldPos - gLightPos)) * EPSILON;
        gl_Position = projection * view * vec4((GSin[2].WorldPos + LightDir), 1.0);
        EmitVertex();

        LightDir = (normalize(GSin[4].WorldPos - gLightPos)) * EPSILON;
        gl_Position = projection * view * vec4((GSin[4].WorldPos + LightDir), 1.0);
        EmitVertex();
        EndPrimitive();
    }
}
