#include "mesh.h"

Mesh::Mesh()
{
	setDrawMode(MESH_DRAW_MODE_TRIANGLE);
	setDrawItem(MESH_DRAW_ITEM_ARRAY);
}

Mesh::Mesh(PhongMaterial material, Geometry geometry) : material(material), geometry(geometry)
{
	setDrawMode(MESH_DRAW_MODE_TRIANGLE);
	setDrawItem(MESH_DRAW_ITEM_ARRAY);
}

Mesh::Mesh(aiMesh * mesh, const Resource::Assimp * assimpResource)
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
	drawModeGl_ = other.drawModeGl_;
	drawItemGl_ = other.drawItemGl_;
	#endif
	drawMode_ = other.drawMode_;
	material = other.material;
	geometry = other.geometry;
	setup_ = other.setup_;
}

Mesh::~Mesh()
{
	freeGeometry();
	freeMaterial();
	freeBuffers();
}

void Mesh::draw()
{
	console::info("empty draw");
}

void Mesh::initFromAi(aiMesh * mesh, const Resource::Assimp * assimpResource)
{
	setDrawMode(MESH_DRAW_MODE_TRIANGLE);
	setDrawItem(MESH_DRAW_ITEM_ELEMENTS);

	freeGeometry();
	freeMaterial();

	// console::warn("mesh name", mesh->mName.C_Str());

	// console::info("mNumBones", mesh->mNumBones);
	// for (unsigned int i = 0; i < mesh->mNumBones; i++) {
	// 	unsigned int boneIndex = 0;
	// 	std::string boneName(mesh->mBones[i]->mName.data);
	// 	console::info("boneName", boneName);
	// }

	console::info("mesh", mesh->mName.C_Str());
	if (mesh->mNumBones > 0) {
		for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			unsigned int boneIndex = 0;
			std::string boneName(mesh->mBones[i]->mName.data);
			// console::info(" ++ boneName", boneName);
		}
	}

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
}

void Mesh::freeGeometry() {}
void Mesh::freeMaterial() {}

void Mesh::setDrawMode(MeshDrawMode mode)
{
	drawMode_ = mode;
	#ifdef GRAPHIC_API_OPENGL
	switch (drawMode_) {
	case MESH_DRAW_MODE_TRIANGLE:
		drawModeGl_ = GL_TRIANGLES;
		break;
	case MESH_DRAW_MODE_LINE:
		drawModeGl_ = GL_LINE_LOOP;
		break;
	}
	#endif
}

void Mesh::setDrawItem(MeshDrawItem item)
{
	#ifdef GRAPHIC_API_OPENGL
	drawItemGl_ = item;
	#endif
}

#ifdef GRAPHIC_API_OPENGL

void Mesh::draw(Opengl::Program &program)
{
	std::vector<Vertex> * vertices = geometry.getVertices();
	std::vector<MeshIndex> * indices = geometry.getIndices();

	unsigned int textureIndex = 0;
	const std::vector<Texture>& textures = material.getTextures();
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
	program.setMat("model", modelMatrix);

	program.setVec("material.ambient", material.ambient);
	program.setVec("material.diffuse", material.diffuse);
	program.setVec("material.specular", material.specular);
	program.setFloat("material.shininess", material.shininess);

	switch (drawItemGl_)
	{
	case MESH_DRAW_ITEM_ARRAY:
		glDrawArrays(drawModeGl_, 0, vertices->size());
		break;
	case MESH_DRAW_ITEM_ELEMENTS:
		glDrawElements(drawModeGl_, indices->size(), GL_UNSIGNED_INT, 0);
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
	if (setup_ == true)
	{
		console::warn("mesh already setuped");
		return;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	std::vector<Vertex> * vertices = geometry.getVertices();

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &vertices->front(), GL_STATIC_DRAW);

	if (drawItemGl_ == MESH_DRAW_ITEM_ELEMENTS)
	{
		glGenBuffers(1, &EBO);
		std::vector<MeshIndex> * indices = geometry.getIndices();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(MeshIndex), &indices->front(), GL_STATIC_DRAW);
	}

	setupVertex(vertices->front());

	glBindVertexArray(0);

	setup_ = true;
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
