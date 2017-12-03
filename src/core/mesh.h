#ifndef MESH_H_
#define MESH_H_

#include "glm/ext.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <assimp/scene.h>
#include "object3D.h"
#include "vertex.h"
#include "../lib/assimp.h"
#include "../materials/material.h"
#include "../resources/resources.h"
#include "geometry.h"
#include "texture.h"
#include "mesh_bone.h"

#ifdef GRAPHIC_API_OPENGL
#include <GL/glew.h>
#include <GL/gl.h>
#include "../renderer/opengl/program.h"
#include "../renderer/opengl/uniforms.h"
#endif

enum MeshDrawType {
	Mesh_Draw_Triangle,
	Mesh_Draw_Line,
	Mesh_Draw_Line_Loop
};

enum MeshDrawMode {
	Mesh_Draw_Arrays,
	Mesh_Draw_Elements
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

	void setDrawType(MeshDrawType type);
	MeshDrawType getDrawType();

	#ifdef GRAPHIC_API_OPENGL
	void draw(Opengl::Program &program);
	void setup();
	void freeBuffers();
	#endif

	std::unordered_map<std::string, MeshBone> bones;
	std::vector<mat4> transforms;

	PhongMaterial material;
	Geometry geometry;
private:
	bool isSetupReady;
	std::string name;
	MeshDrawType drawType;
};

#endif
