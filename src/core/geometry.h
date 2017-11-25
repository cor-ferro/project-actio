#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <vector>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/constants.hpp>
#include "../lib/types.h"
#include "../lib/console.h"
#include "../lib/mem.h"
#include "../resources/resources.h"
#include "vertex.h"

typedef unsigned int MeshIndex;
typedef std::vector<Vertex> GeometryVertices;
typedef std::vector<MeshIndex> GeometryIndices;


static size_t TotalCountVertices = 0;

struct GeometryCone {
	float radiusTop;
	float radiusBottom;
	float height;
	uint radialSegments;
	uint heightSegments;
	bool openEnded;
	float thetaStart;
	float thetaLength;
};

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
	static Geometry Plane(uint width, uint height, uint widthSegments, uint heightSegments);
	static Geometry Sphere(float radius, uint widthSegments, uint heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength);
	static Geometry Circle(float radius, uint segments, float thetaStart, float thetaLength);
	static Geometry Cone(float radius, float height, uint radialSegments, uint heightSegments, bool openEnded, float thetaStart, float thetaLength);
	static void CylinderTorso(GeometryCone& params, Geometry& geometry);
	static void CylinderCap(GeometryCone& params, Geometry& geometry, bool top);
	static Geometry Cylinder(float radiusTop, float radiusBottom, float height, uint radialSegments, uint heightSegments, bool openEnded, float thetaStart, float thetaLength);
	static Geometry Ring(float innerRadius, float outerRadius, uint thetaSegments, uint phiSegments, float thetaStart, float thetaLength);
	static Geometry Torus(float radius, float tube, uint radialSegments, uint tubularSegments, float arc);
	static Geometry Octahedron(float radius);

	void initFromAi(aiMesh * mesh, const Resource::Assimp * assimpResource);
	GeometryVertices * getVertices();
	GeometryIndices * getIndices();

	void addVertex(Vertex vertex);
	void addVertex(float x, float y, float z);
	void addVertex(std::vector<float>& vertices);
	Vertex& getVertex(unsigned int index);

	void fillIndices();
	void addIndex(unsigned int i);
	void addFace(unsigned int i1, unsigned int i2, unsigned int i3);

protected:
	void allocVertices(unsigned int count);
	void allocIndices(unsigned int count);
	void freeVerties();
	void freeIndices();

	std::shared_ptr<GeometryVertices> vertices_;
	std::shared_ptr<GeometryIndices> indices_;
};

#endif
