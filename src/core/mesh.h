#ifndef MESH_H_
#define MESH_H_

#include <glad/glad.h>
#include "glm/ext.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <assimp/scene.h>
#include "object3D.h"
#include "vertex.h"
#include "../lib/assimp.h"
#include "../lib/colors.h"
#include "../materials/material.h"
#include "../resources/resources.h"
#include "geometry.h"
#include "texture.h"
#include "mesh_bone.h"

#ifdef GRAPHIC_API_OPENGL
#include "../renderer/opengl/program.h"
#include "../renderer/opengl/uniforms.h"
#include "../renderer/opengl/utils.h"
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

enum MeshDrawFlags {
	Mesh_Draw_Base = 0x1,
	Mesh_Draw_Textures = 0x2,
	Mesh_Draw_Bones = 0x4,
	Mesh_Draw_Material = 0x8,
	Mesh_Draw_All = Mesh_Draw_Base | Mesh_Draw_Textures | Mesh_Draw_Bones | Mesh_Draw_Material
};

struct Mesh : Object3D {
	Mesh();
	Mesh(Geometry geometry);
	Mesh(Material::Phong material, Geometry geometry);
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
	void draw(Opengl::Program &program, uint flags = Mesh_Draw_All);
	void setup();
	void freeBuffers();
	#endif

	std::unordered_map<std::string, MeshBone> bones;
	std::vector<mat4> transforms;

	Material::Phong material;
	Geometry geometry;
private:
	bool isSetupReady;
	std::string name;
	MeshDrawType drawType;
};

#endif
