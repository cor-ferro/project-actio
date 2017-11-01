#include "vertex.h"

Vertex::Vertex()
{
	Position = vec3(0.0f);
	Normal = vec3(0.0f);
	TexCoords = vec2(1.0f);
	Tangent = vec3(1.0f);
	Bitangent = vec3(1.0f);
}

Vertex::Vertex(float x, float y, float z)
{
	Position = vec3(x, y, z);
	Normal = vec3(0.0f);
	TexCoords = vec2(1.0f);
	Tangent = vec3(1.0f);
	Bitangent = vec3(1.0f);
}

Vertex::Vertex(vec3 vector)
{
	Position = vector;
	Normal = vec3(0.0f);
	TexCoords = vec2(1.0f);
	Tangent = vec3(1.0f);
	Bitangent = vec3(1.0f);
}

Vertex::Vertex(const Vertex & other)
{
	Position = other.Position;
	Normal = other.Normal;
	TexCoords = other.TexCoords;
	Tangent = other.Tangent;
	Bitangent = other.Bitangent;
}
