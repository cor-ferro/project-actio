#ifndef VERTEX_H_
#define VERTEX_H_

#include <iostream>
#include <glm/glm.hpp>
#include "../lib/types.h"

struct Vertex {
	Vertex();
	Vertex(float x, float y, float z);
	Vertex(vec3 vector);
	Vertex(const Vertex& vertex);

	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
	vec3 Tangent;
	vec3 Bitangent;
	vec4 BonedIDs;
	vec4 Weights;
};

struct Vertex1 {
	vec3 Position;
};

struct Vertex2 {
	vec3 Position;
	vec3 Normal;
};

struct VertexBone {
	uint IDs[4];
	float Weights[4];
};

#endif
