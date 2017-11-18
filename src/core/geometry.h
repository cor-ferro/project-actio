#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <vector>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/normal.hpp>
#include "../lib/types.h"
#include "../lib/console.h"
#include "../lib/mem.h"
#include "../resources/resources.h"
#include "vertex.h"

typedef unsigned int MeshIndex;
typedef std::vector<Vertex> GeometryVertices;
typedef std::vector<MeshIndex> GeometryIndices;


static size_t TotalCountVertices = 0;

struct Geometry {
	Geometry();
	Geometry(aiMesh * mesh, const Resource::Assimp * assimpResource);
	Geometry(const Geometry& geometry);
	~Geometry();

	static int VertexSize() { return sizeof(Vertex); }
	static void AddTotalCountVertices(size_t count) { TotalCountVertices+= count; }
	static void SubTotalCountVertices(size_t count) { TotalCountVertices-= count; }
	static size_t GetTotalCountVertices() {	return TotalCountVertices; }

	static Geometry Box();

	void initFromAi(aiMesh * mesh, const Resource::Assimp * assimpResource);
	GeometryVertices * getVertices();
	GeometryIndices * getIndices();

	void addVertex(Vertex vertex);
	void addVertex(float x, float y, float z);
	void addVertex(std::vector<float>& vertices);
	Vertex& getVertex(unsigned int index);

	void fillIndices();

protected:
	void allocVertices(unsigned int count);
	void allocIndices(unsigned int count);
	void freeVerties();
	void freeIndices();

	std::shared_ptr<GeometryVertices> vertices_;
	std::shared_ptr<GeometryIndices> indices_;
};

#endif
