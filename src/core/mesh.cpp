#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/ext.hpp>
#include "../lib/assimp.h"
#include "../lib/colors.h"
#include "mesh.h"

Mesh::Mesh()
	: name("")
	, id(newMeshId())
	, drawType(Mesh_Draw_Triangle)
{}

//Mesh::Mesh(Geometry geometry)
//	: name("")
//	, id(newMeshId())
//	, geometry(geometry)
//	, drawType(Mesh_Draw_Triangle)
//{}
//
//Mesh::Mesh(Geometry geometry, Material::Phong material)
//	: name("")
//	, id(newMeshId())
//	, material(material)
//	, geometry(geometry)
//	, drawType(Mesh_Draw_Triangle)
//{}

Mesh::Mesh(const Mesh& other)
{
	freeGeometry();
	freeMaterial();

	console::info("copy mesh");

	name = other.name;
	material = other.material;
	geometry = other.geometry;
	drawType = other.drawType;
}

Mesh * Mesh::Create()
{
	void * place = meshAllocator->Allocate(sizeof(Mesh), 8);
	Mesh * mesh = new(place) Mesh();

	return mesh;
}

//Mesh * Mesh::Create(Geometry geometry)
//{
//	void * place = meshAllocator->Allocate(sizeof(Mesh), 8);
//	Mesh * mesh = new(place) Mesh(geometry);
//
//	return mesh;
//}
//
//Mesh * Mesh::Create(Geometry geometry, Material::Phong material)
//{
//	void * place = meshAllocator->Allocate(sizeof(Mesh), 8);
//	Mesh * mesh = new(place) Mesh(geometry, material);
//
//	return mesh;
//}

void Mesh::Destroy(Mesh * mesh)
{
	if (mesh == nullptr) return;

	mesh->destroy();
	mesh->~Mesh();
	meshAllocator->Free((void*)mesh);
}

Mesh::~Mesh()
{
//	console::info("free mesh %i", id);
}

void Mesh::destroy()
{
	geometry.destroy(); // @todo: удаление ресурсов не работает корректно, из за переноса логики setup в рендер
	material.destroy();
}

void Mesh::freeGeometry() {}
void Mesh::freeMaterial() {}

std::string Mesh::getName()
{
	return name;
}

MeshId Mesh::getId()
{
	return id;
}

void Mesh::setName(std::string newName)
{
	name = newName;
}

void Mesh::setName(const char * newName)
{
	name = std::string(newName);
}

void Mesh::setDrawType(MeshDrawType type)
{
	drawType = type;
}

MeshDrawType Mesh::getDrawType()
{
	return drawType;
}

void Mesh::draw()
{
	console::info("empty draw");
}

void Mesh::draw(renderer::Opengl::Program& program, uint flags)
{
	if ((flags & Mesh_Draw_Textures) != 0) {
		unsigned int textureIndex = 0;
		const MaterialTextures& textures = material.getTextures();
		for (const Texture& texture : textures)
		{
			OpenglUtils::bindTexture(GL_TEXTURE0 + textureIndex, texture);
			program.setInt(texture.name, textureIndex);
			textureIndex++;
		}
	}

	if ((flags & Mesh_Draw_Material) != 0) {
		program.setVec(renderer::Opengl::Uniform::MaterialAmbient, material.ambient);
		program.setVec(renderer::Opengl::Uniform::MaterialDiffuse, material.diffuse);
		program.setVec(renderer::Opengl::Uniform::MaterialSpecular, material.specular);
		program.setFloat(renderer::Opengl::Uniform::MaterialShininess, material.shininess);
	}

//	if ((flags & Mesh_Draw_Bones) != 0) {
//		program.enableVertexSubroutine("BoneTransformEnabled");
//		program.setMat("bones[]", &boneTransforms);
//	} else {
		program.enableVertexSubroutine("getBoneTransform", "BoneTransformDisabled");
//	}

	if ((flags & Mesh_Draw_Base) != 0) {
		glBindVertexArray(geometry.VAO);

		updateModelMatrix(false);
		program.setMat("model", modelMatrix);

		GeometryVertices * vertices = geometry.getVertices();
		GeometryIndices * indices = geometry.getIndices();

		GLenum primitiveType;
		switch (drawType) {
			case Mesh_Draw_Line:
				primitiveType = GL_LINES; break;
			case Mesh_Draw_Line_Loop:
				primitiveType = GL_LINE_LOOP; break;
			case Mesh_Draw_Triangle:
				primitiveType = GL_TRIANGLES; break;
			default:
				primitiveType = GL_TRIANGLES;
		}

		MeshDrawMode drawMode = indices->size() != 0 ? Mesh_Draw_Elements : Mesh_Draw_Arrays;

		if (material.wireframe == true) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		switch (drawMode) {
			case Mesh_Draw_Arrays: 		glDrawArrays(primitiveType, 0, vertices->size()); break;
			case Mesh_Draw_Elements: 	glDrawElements(primitiveType, indices->size(), GL_UNSIGNED_INT, 0);	break;
			default: console::warn("unknown draw mode: %i", drawMode);
		}

		if (material.wireframe == true) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(0);
	}
}

void Mesh::setup()
{
	geometry.setup();
}

void Mesh::setDrawStride(unsigned int value) {
	drawStride = value;
};
unsigned int Mesh::getDrawStride() {
	return drawStride;
};