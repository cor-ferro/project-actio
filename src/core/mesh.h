#ifndef MESH_H_
#define MESH_H_

#include "glm/ext.hpp"

#include <vector>
#include <unordered_map>
#include <map>
#include <assimp/scene.h>
#include "object3D.h"
#include "vertex.h"
#include "../lib/assimp.h"
#include "../materials/material.h"
#include "../resources/resources.h"
#include "geometry.h"
#include "texture.h"

#ifdef GRAPHIC_API_OPENGL
#include <GL/glew.h>
#include <GL/gl.h>
#include "../renderer/opengl/program.h"
#endif


typedef std::pair<uint, float> BoneVertexWeight;
typedef std::map<uint, std::vector<BoneVertexWeight>> VerticesMap;

enum MeshDrawMode {
	MESH_DRAW_MODE_TRIANGLE,
	MESH_DRAW_MODE_LINE
};

enum MeshDrawItem {
	MESH_DRAW_ITEM_ARRAY,
	MESH_DRAW_ITEM_ELEMENTS
};

struct MeshBone {
	MeshBone();
	MeshBone(aiBone * bone);

	unsigned int getIndex() const;
	mat4 getTransform() const;
	mat4 getOffset() const;

	void setOffset(mat4& newOffset);
	void setTransform(mat4& newTransform);
	void setIndex(unsigned int& newIndex);

private:
	unsigned int index;
	mat4 offset;
	mat4 transform;
};

struct Mesh : Object3D {
	Mesh();
	Mesh(PhongMaterial material, Geometry geometry);
	Mesh(aiMesh * mesh, const Resource::Assimp * assimpResource);
	Mesh(const Mesh& mesh);
	~Mesh();

	std::string getName();
	void setName(std::string newName);
	void setName(const char * newName);

	void initFromAi(aiMesh * mesh, const Resource::Assimp * assimpResource);

	void draw();

	void freeGeometry();
	void freeMaterial();

	void setDrawMode(MeshDrawMode mode);
	void setDrawItem(MeshDrawItem item);

	#ifdef GRAPHIC_API_OPENGL
	void draw(Opengl::Program &program);
	void setup();
	void freeBuffers();
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	#endif

	std::unordered_map<std::string, MeshBone> bones;
	std::vector<mat4> transforms;

	PhongMaterial material;
	Geometry geometry;
private:
	void setupVertex(Vertex& v);
	void setupVertex(Vertex1& v);
	void setupVertex(Vertex2& v);

	bool isSetupReady;
	std::string name;
	MeshDrawMode drawMode;
	#ifdef GRAPHIC_API_OPENGL
	GLenum drawModeGl;
	MeshDrawItem drawItemGl;
	#endif
};

#endif
