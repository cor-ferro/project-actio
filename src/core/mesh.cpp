#include "mesh.h"

Mesh::Mesh()
	: name("")
	, drawType(Mesh_Draw_Triangle)
{}

Mesh::Mesh(Geometry geometry)
	: name("")
	, geometry(geometry)
	, drawType(Mesh_Draw_Triangle)
{}

Mesh::Mesh(Geometry geometry, Material::Phong material)
	: name("")
	, material(material)
	, geometry(geometry)
	, drawType(Mesh_Draw_Triangle)
{}

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

Mesh * Mesh::Create(Geometry geometry)
{
	void * place = meshAllocator->Allocate(sizeof(Mesh), 8);
	Mesh * mesh = new(place) Mesh(geometry);

	return mesh;
}

Mesh * Mesh::Create(Geometry geometry, Material::Phong material)
{
	void * place = meshAllocator->Allocate(sizeof(Mesh), 8);
	Mesh * mesh = new(place) Mesh(geometry, material);

	return mesh;
}

void Mesh::Destroy(Mesh * mesh)
{
	if (mesh == nullptr) return;

	mesh->destroy();
	mesh->~Mesh();
	meshAllocator->Free((void*)mesh);
}

Mesh::~Mesh()
{
	console::info("free mesh");
}

void Mesh::destroy()
{
	geometry.destroy();
	material.destroy();
}

void Mesh::freeGeometry() {}
void Mesh::freeMaterial() {}

std::string Mesh::getName()
{
	return name;
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

#ifdef GRAPHIC_API_OPENGL

void Mesh::draw(Opengl::Program& program, uint flags)
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
		program.setVec(Opengl::Uniform::MaterialAmbient, material.ambient);
		program.setVec(Opengl::Uniform::MaterialDiffuse, material.diffuse);
		program.setVec(Opengl::Uniform::MaterialSpecular, material.specular);
		program.setFloat(Opengl::Uniform::MaterialShininess, material.shininess);
	}

//	if ((flags & Mesh_Draw_Bones) != 0) {
//		std::string boneTransformType;
//		if (bones.size() > 0) {
//			boneTransformType = "BoneTransformEnabled";
//			std::vector<mat4> * boneTransforms = &transforms;
//
//			program.setMat(Opengl::Uniform::Bones, boneTransforms);
//		} else {
//			boneTransformType = "BoneTransformDisabled";
//		}
//
//		program.enableVertexSubroutine(boneTransformType);
//	}

	if ((flags & Mesh_Draw_Base) != 0) {
		glBindVertexArray(geometry.VAO);

		updateModelMatrix(false);
		program.setMat("model", finalMatrix);

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
			default: console::warn("unknown draw mode: ", drawMode);
		}

		if (material.wireframe == true) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(0);
	}
}

void Mesh::setup()
{
	geometry.setup();
}

#endif
