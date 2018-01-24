#include "mesh.h"

Mesh::Mesh()
	: name("")
	, isSetupReady(false)
	, drawType(Mesh_Draw_Triangle)
{}

Mesh::Mesh(Geometry geometry) 
	: name("")
	, isSetupReady(false)
	, geometry(geometry)
	, drawType(Mesh_Draw_Triangle)
{}

Mesh::Mesh(PhongMaterial material, Geometry geometry) 
	: name("")
	, isSetupReady(false)
	, material(material)
	, geometry(geometry)
	, drawType(Mesh_Draw_Triangle)
{}

Mesh::Mesh(aiMesh * mesh, const Resource::Assimp * assimpResource)
	: name("")
	, isSetupReady(false)
	, drawType(Mesh_Draw_Triangle)
{
	initFromAi(mesh, assimpResource);
}

Mesh::Mesh(const Mesh& other)
{
	freeGeometry();
	freeMaterial();

	freeBuffers();

	console::info("copy mesh");

	name = other.name;
	material = other.material;
	geometry = other.geometry;
	isSetupReady = other.isSetupReady;
	bones = other.bones;
	transforms = other.transforms;
	drawType = other.drawType;
}

Mesh::~Mesh()
{
	// freeGeometry();
	// freeMaterial();
	// freeBuffers();
}

void Mesh::initFromAi(aiMesh * mesh, const Resource::Assimp * assimpResource)
{
	freeGeometry();
	freeMaterial();

	setName(mesh->mName.C_Str());

	if (mesh->mMaterialIndex >= 0) {
		material.initFromAi(assimpResource->scene->mMaterials[mesh->mMaterialIndex], assimpResource);
	} else{
		console::warn("skip init material", assimpResource->resourcePath_);
		material.initEmptyTextures();
		material.setAmbient(0.0f, 1.0f, 0.0f);
		material.setDiffuse(0.0f, 1.0f, 0.0f);
		material.setSpecular(0.0f, 1.0f, 0.0f);
	}

	if (mesh->mNumVertices > 0) {
	geometry.initFromAi(mesh, assimpResource);
	}

	if (mesh->mNumBones > 0) {
		console::info("bones count: ", mesh->mNumBones);
		transforms.reserve(mesh->mNumBones);
		GeometryVertices * vertices = geometry.getVertices();
		const int verticesCount = vertices->size();

		std::unique_ptr<VerticesMap> verticesMap(new VerticesMap());

		for (unsigned int boneId = 0; boneId < mesh->mNumBones; boneId++) {
			aiBone * bone = mesh->mBones[boneId];

			MeshBone meshBone(bone);

			std::string boneName(bone->mName.data);
			mat4 offsetBone = libAi::toNativeType(bone->mOffsetMatrix);

			meshBone.setIndex(boneId);

			bones.insert(
				std::pair<std::string, MeshBone>(boneName, meshBone)
			);

			transforms.push_back(mat4(0.0f));
			
			// console::info("bone ", boneName, " ", bone->mNumWeights);
			for (unsigned int j = 0; j < bone->mNumWeights; j++) {
				const aiVertexWeight vertexWeight = bone->mWeights[j];

				auto vertexIt = verticesMap->find(vertexWeight.mVertexId);

				if (vertexIt != verticesMap->end()) {
					vertexIt->second.push_back(BoneVertexWeight(boneId, vertexWeight.mWeight));
				} else {
					std::vector<BoneVertexWeight> mapping { BoneVertexWeight(boneId, vertexWeight.mWeight) };
					verticesMap->insert({ vertexWeight.mVertexId, mapping });
				}
			}
		}

		for (auto it = verticesMap->begin(); it != verticesMap->end(); ++it) {
			const uint& vertexId = it->first;
			std::vector<BoneVertexWeight>& boneWeights = it->second;

			Vertex& vertex = geometry.getVertex(vertexId);

			unsigned int boneMappingIndex = 0;
			for (auto itBoneWeights = boneWeights.begin(); itBoneWeights != boneWeights.end(); itBoneWeights++) {
				vertex.BonedIDs[boneMappingIndex] = itBoneWeights->first;
				vertex.Weights[boneMappingIndex] = itBoneWeights->second;

				boneMappingIndex++;
				if (boneMappingIndex > 4) {
					console::info("boneWeights.size() ", boneWeights.size());
				}
				assert(boneMappingIndex <= 4);
			}
		}
	}
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

	if ((flags & Mesh_Draw_Bones) != 0) {
		std::string boneTransformType;
		if (bones.size() > 0) {
			boneTransformType = "BoneTransformEnabled";
			std::vector<mat4> * boneTransforms = &transforms;

			program.setMat(Opengl::Uniform::Bones, boneTransforms);
		} else {
			boneTransformType = "BoneTransformDisabled";
		}

		program.enableVertexSubroutine(boneTransformType);
	}

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
			default: console::warn("unknown draw mode: ", drawMode);
		}

		if (material.wireframe == true) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glBindVertexArray(0);
	}
}

void Mesh::setup()
{
	if (isSetupReady == true)
	{
		console::warn("mesh already setup");
		return;
	}

	geometry.setup();

	isSetupReady = true;
}

void Mesh::freeBuffers()
{
	// glDeleteBuffers(1, &VBO);
	// glDeleteBuffers(1, &EBO);
	// glDeleteVertexArrays(1, &VAO);
}


#endif
