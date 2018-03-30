#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "../materials/material.h"
#include "../resources/resources.h"
#include "../memory/poolallocator.h"
#include "geometry.h"
#include "texture.h"
#include "mesh_bone.h"
#include "object3D.h"
#include "vertex.h"

#ifdef GRAPHIC_API_OPENGL
#include "../renderer/opengl/program.h"
#include "../renderer/opengl/uniforms.h"
#include "../renderer/opengl/utils.h"
#endif

extern memory::PoolAllocator * meshAllocator;

typedef std::size_t MeshId;

static size_t idMeshCounter = 0;
static MeshId newMeshId()
{
	return ++idMeshCounter;
}

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
	static Mesh * Create();
	static Mesh * Create(Geometry geometry);
	static Mesh * Create(Geometry geometry, Material::Phong material);
	static void Destroy(Mesh * mesh);

	~Mesh();

	std::string getName();
	MeshId getId();
	void setName(std::string newName);
	void setName(const char * newName);

	void draw();

	void freeGeometry();
	void freeMaterial();

	void setDrawType(MeshDrawType type);
	MeshDrawType getDrawType();

	#ifdef GRAPHIC_API_OPENGL
	void draw(renderer::Opengl::Program &program, uint flags = Mesh_Draw_All);
	void setup();
	#endif

	Material::Phong material;
	Geometry geometry;

	std::vector<mat4> boneTransforms;
private:
	Mesh();
	Mesh(Geometry geometry);
	Mesh(Geometry geometry, Material::Phong material);
	Mesh(const Mesh& mesh);

	void destroy();

	std::size_t id;
	std::string name;
	MeshDrawType drawType;
};

#endif
