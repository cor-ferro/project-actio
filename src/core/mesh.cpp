#include "mesh.h"

MeshBone::MeshBone()
	: offset(mat4(1.0))
	, transform(mat4(1.0))
	, index(0)
{

}

MeshBone::MeshBone(aiBone * bone) 
	: offset(aiMatToMat(bone->mOffsetMatrix))
	, transform(mat4(1.0))
	, index(0)
{

}

unsigned int MeshBone::getIndex() const
{
	return index;
}

mat4 MeshBone::getTransform() const
{
	return transform;
}

mat4 MeshBone::getOffset() const
{
	return offset;
}

void MeshBone::setOffset(mat4& newOffset)
{
	offset = newOffset;
}

void MeshBone::setTransform(mat4& newTransform)
{
	transform = newTransform;
}

void MeshBone::setIndex(unsigned int& newIndex)
{
	index = newIndex;
}


Mesh::Mesh()
	: name("")
	, isSetupReady(false)
{
	setDrawMode(MESH_DRAW_MODE_TRIANGLE);
	setDrawItem(MESH_DRAW_ITEM_ARRAY);
}

Mesh::Mesh(PhongMaterial material, Geometry geometry) 
	: name("")
	, isSetupReady(false)
	, material(material)
	, geometry(geometry)
{
	setDrawMode(MESH_DRAW_MODE_TRIANGLE);
	setDrawItem(MESH_DRAW_ITEM_ARRAY);
}

Mesh::Mesh(aiMesh * mesh, const Resource::Assimp * assimpResource)
	: name("")
	, isSetupReady(false)
{
	setDrawMode(MESH_DRAW_MODE_TRIANGLE);
	setDrawItem(MESH_DRAW_ITEM_ELEMENTS);
	initFromAi(mesh, assimpResource);
}

Mesh::Mesh(const Mesh& other)
{
	freeGeometry();
	freeMaterial();

	freeBuffers();

	console::info("copy mesh");
	#ifdef GRAPHIC_API_OPENGL
	VAO = other.VAO;
	VBO = other.VBO;
	EBO = other.EBO;
	drawModeGl = other.drawModeGl;
	drawItemGl = other.drawItemGl;
	#endif
	name = other.name;
	drawMode = other.drawMode;
	material = other.material;
	geometry = other.geometry;
	isSetupReady = other.isSetupReady;
	bones = other.bones;
	transforms = other.transforms;
}

Mesh::~Mesh()
{
	freeGeometry();
	freeMaterial();
	freeBuffers();
}

void Mesh::initFromAi(aiMesh * mesh, const Resource::Assimp * assimpResource)
{
	setDrawMode(MESH_DRAW_MODE_TRIANGLE);
	setDrawItem(MESH_DRAW_ITEM_ELEMENTS);

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

	geometry.initFromAi(mesh, assimpResource);

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
			mat4 offsetBone = aiMatToMat(bone->mOffsetMatrix);

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
					verticesMap->insert(
						std::pair<uint, std::vector<BoneVertexWeight>>(vertexWeight.mVertexId, mapping)
					);
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

void Mesh::setDrawMode(MeshDrawMode mode)
{
	drawMode = mode;
	#ifdef GRAPHIC_API_OPENGL
	switch (drawMode) {
	case MESH_DRAW_MODE_TRIANGLE:
		drawModeGl = GL_TRIANGLES;
		break;
	case MESH_DRAW_MODE_LINE:
		drawModeGl = GL_LINE_LOOP;
		break;
	}
	#endif
}

void Mesh::setDrawItem(MeshDrawItem item)
{
	#ifdef GRAPHIC_API_OPENGL
	drawItemGl = item;
	#endif
}

void Mesh::draw()
{
	console::info("empty draw");
}

#ifdef GRAPHIC_API_OPENGL

void Mesh::draw(Opengl::Program &program)
{
	GeometryVertices * vertices = geometry.getVertices();
	GeometryIndices * indices = geometry.getIndices();

	unsigned int textureIndex = 0;
	const MaterialTextures& textures = material.getTextures();
	for (const Texture& texture : textures)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(texture.textureTarget, texture.textureID);
		program.setInt(texture.name, textureIndex);
		textureIndex++;
	}

	glBindVertexArray(VAO);

	if (material.wireframe == true)
	{
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}

	updateModelMatrix(false);

	std::string boneTransformType;

	if (bones.size() > 0) {
		boneTransformType = "BoneTransformEnabled";
		std::vector<mat4> * boneTransforms = &transforms;

		program.setMat("bones[]", boneTransforms);
	} else {
		boneTransformType = "BoneTransformDisabled";
	}

	program.enableVertexSubroutine(boneTransformType);

	program.setMat("model", modelMatrix);

	program.setVec("material.ambient", material.ambient);
	program.setVec("material.diffuse", material.diffuse);
	program.setVec("material.specular", material.specular);
	program.setFloat("material.shininess", material.shininess);

	switch (drawItemGl)
	{
	case MESH_DRAW_ITEM_ARRAY:
		glDrawArrays(drawModeGl, 0, vertices->size());
		break;
	case MESH_DRAW_ITEM_ELEMENTS:
		glDrawElements(drawModeGl, indices->size(), GL_UNSIGNED_INT, 0);
		break;
	}

	if (material.wireframe == true)
	{
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}

	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setup()
{
	if (isSetupReady == true)
	{
		console::warn("mesh already setup");
		return;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	GeometryVertices * vertices = geometry.getVertices();

	// console::info("vertices size", vertices->size());

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &vertices->front(), GL_STATIC_DRAW);

	if (drawItemGl == MESH_DRAW_ITEM_ELEMENTS)
	{
		glGenBuffers(1, &EBO);
		GeometryIndices * indices = geometry.getIndices();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(MeshIndex), &indices->front(), GL_STATIC_DRAW);
	}

	setupVertex(vertices->front());

	glBindVertexArray(0);

	isSetupReady = true;
}

void Mesh::setupVertex(Vertex& v)
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BonedIDs));
	// glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BonedIDs));

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Weights));
}

void Mesh::setupVertex(Vertex1& v)
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
}

void Mesh::setupVertex(Vertex2& v)
{
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
}

void Mesh::freeBuffers()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}


#endif
