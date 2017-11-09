#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <assimp/scene.h>
#include "object3D.h"
#include "vertex.h"
#include "../materials/material.h"
#include "../resources/resources.h"
#include "geometry.h"
#include "texture.h"

#ifdef GRAPHIC_API_OPENGL
#include <GL/glew.h>
#include <GL/gl.h>
#include "../renderer/opengl/program.h"
#endif

enum MeshDrawMode {
	MESH_DRAW_MODE_TRIANGLE,
	MESH_DRAW_MODE_LINE
};

enum MeshDrawItem {
	MESH_DRAW_ITEM_ARRAY,
	MESH_DRAW_ITEM_ELEMENTS
};

struct Mesh : Object3D {
	Mesh();
	Mesh(PhongMaterial material, Geometry geometry);
	Mesh(aiMesh * mesh, const Resource::Assimp * assimpResource);
	Mesh(const Mesh& mesh);
	~Mesh();

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

	PhongMaterial material;
	Geometry geometry;
private:
	bool setup_ = false;
	MeshDrawMode drawMode_;
	#ifdef GRAPHIC_API_OPENGL
	GLenum drawModeGl_;
	MeshDrawItem drawItemGl_;
	#endif
	void setupVertex(Vertex& v);
	void setupVertex(Vertex1& v);
	void setupVertex(Vertex2& v);
};

#endif
