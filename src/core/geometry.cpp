#include "geometry.h"

Geometry::Geometry() {
	vertices_.reset(new GeometryVertices());
	indices_.reset(new GeometryIndices());

	allocVertices(1);
	allocIndices(1);
}

Geometry::Geometry(aiMesh * mesh, const Resource::Assimp * assimpResource) {
	vertices_.reset(new GeometryVertices());
	indices_.reset(new GeometryIndices());

	initFromAi(mesh, assimpResource);
}

Geometry::Geometry(const Geometry& other)
{
	vertices_ = other.vertices_;
	indices_ = other.indices_;

	console::info("geometry copy vertices_", vertices_->size());
}

Geometry::~Geometry()
{
}

void Geometry::initFromAi(aiMesh * mesh, const Resource::Assimp * assimpResource)
{
	console::info("geometry vertices: ", mesh->mNumVertices);

	unsigned int numVertices = mesh->mNumVertices;
	unsigned int numBones = mesh->mNumBones;
	allocVertices(numVertices);

	GetTotalCountVertices(); // why?

	for (unsigned int i = 0; i < numVertices; i++)
	{
		Vertex vertex;

		vertex.Position = vec3(
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z
		);
		vertex.Normal = vec3(
				mesh->mNormals[i].x,
				mesh->mNormals[i].y,
				mesh->mNormals[i].z
		);

		if (mesh->mTextureCoords[0])
		{
			vertex.TexCoords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.TexCoords = vec2(0.1f, 0.1f);
		}

		if (mesh->mTangents != nullptr) {
			vertex.Tangent = vec3(mesh->mTangents[i].x,	mesh->mTangents[i].y, mesh->mTangents[i].z);
			vertex.Bitangent = vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
		} else {
			vertex.Tangent = vec3(1.0f);
			vertex.Bitangent = vec3(1.0f);
		}

		addVertex(vertex);
	}

	// console::info("+++ geometry init ", mesh->mName.C_Str(), " - ", numBones);

	// for (unsigned int boneId = 0; boneId < numBones; boneId++) {
	// 	aiBone * bone = mesh->mBones[boneId];

	// 	for (unsigned int j = 0; j < bone->mNumWeights; j++) {
	// 		const aiVertexWeight vertexWeight = bone->mWeights[j];

	// 		vertices_->at(vertexWeight.mVertexId).BonedIDs = vec4(0, 0, 0, 0);
	// 		vertices_->at(vertexWeight.mVertexId).Weights = vec4(1.0f, 0.0f, 0.0f, 0.0f);
	// 	}
	// }

	unsigned int numFaces = mesh->mNumFaces;
	freeIndices();
	allocIndices(numFaces);

	for (unsigned int i = 0; i < numFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices_->push_back(face.mIndices[j]);
		}
	}
}

GeometryVertices * Geometry::getVertices()
{
	return vertices_.get();
}

GeometryIndices * Geometry::getIndices()
{
	return indices_.get();
}

void Geometry::addVertex(Vertex vertex)
{
	vertices_->push_back(vertex);
	AddTotalCountVertices(1);
}

void Geometry::addVertex(float x, float y, float z)
{
	Vertex v(x, y, z);
	vertices_->push_back(v);
	AddTotalCountVertices(1);
}

void Geometry::addVertex(std::vector<float>& vertices)
{
	int verticesSize = vertices.size();
	for (int i = 0; i < verticesSize; i+=3)
	{
		addVertex(vertices[i], vertices[i+1], vertices[i+2]);
	}
}

Vertex& Geometry::getVertex(unsigned int index)
{
	return vertices_->at(index);
}

void Geometry::allocVertices(unsigned int count)
{
	vertices_->reserve(count);
}

void Geometry::allocIndices(unsigned int count)
{
	indices_->reserve(count);
}

void Geometry::fillIndices()
{
	allocIndices(vertices_->size());
	for (int i = 0; i < vertices_->size(); i++)
	{
		Vertex& primaryVertex = vertices_->at(i);

		for (int j = 0; j < vertices_->size(); j++)
		{
			if (j == i) {
				indices_->push_back(i);
				continue;
			}

			Vertex& secondaryVertex = vertices_->at(j);

			if (glm::all(glm::epsilonEqual(primaryVertex.Position, secondaryVertex.Position, 0.01f)))
			{
				indices_->push_back(i);
			}
			else
			{
				indices_->push_back(j);
			}
		}
	}
}

void Geometry::freeVerties() {
	SubTotalCountVertices(vertices_->size());
	vertices_->clear();
}

void Geometry::freeIndices() {
	indices_->clear();
	// if (indices_ != nullptr) {
	// 	delete indices_;
	// 	indices_ = nullptr;
	// }
}

// Geometry Geometry::Box() {
// 	Geometry geometry;

// 	const int verticesCount = 8 * 3;
// 	float vertices[verticesCount] {
// 		1.0f, -1.0f, -1.0f,
// 		1.0f, -1.0f, 1.0f,
// 		-1.0f, -1.0f, 1.0f,
// 		-1.0f, -1.0f, -1.0f,
// 		1.0f, 1.0f, -1.0f,
// 		1.0f, 1.0f, 1.0f,
// 		-1.0f, 1.0f, 1.0f,
// 		-1.0f, 1.0f, -1.0f
// 	};

// 	const int normalsCount = 6 * 3;
// 	float normals[normalsCount] {
// 		0.0f, -1.0f, 0.0f,
// 		0.0f, 1.0f, 0.0f,
// 		1.0f, 0.0f, 0.0f,
// 		0.0f, 0.0f, 1.0f,
// 		-1.0f, -0.0f, 0.0f,
// 		0.0f, 0.0f, -1.0f
// 	};

// 	const int facesCount = 12 * 3;
// 	const int faceSize = 2;
// 	float faces[facesCount][faceSize] = {
// 		{2,1}, {4,1}, {1,1},
// 		{8,2}, {6,2}, {5,2},
// 		{5,3}, {2,3}, {1,3},
// 		{6,4}, {3,4}, {2,4},
// 		{3,5}, {8,5}, {4,5},
// 		{1,6}, {8,6}, {5,6},
// 		{2,1}, {3,1}, {4,1},
// 		{8,2}, {7,2}, {6,2},
// 		{5,3}, {6,3}, {2,3},
// 		{6,4}, {7,4}, {3,4},
// 		{3,5}, {7,5}, {8,5},
// 		{1,6}, {4,6}, {8,6}
// 	};

// 	geometry.freeVerties();
// 	geometry.freeIndices();
// 	geometry.allocVertices(verticesCount);

// 	for (int i = 0; i < facesCount; i++) {
// 		char vertexIndex = faces[i][0];
// 		char normalIndex = faces[i][1];

// 		vec3 vertex = vec3(vertices[vertexIndex], vertices[vertexIndex + 1], vertices[vertexIndex + 2]);
// 		vec3 normal = vec3(normals[normalIndex], normals[normalIndex + 1], normals[normalIndex + 2]);

// 		Vertex face;

// 		face.Position = vertex;
// 		face.Normal = normal;
// 		face.TexCoords = vec2(0.1f, 0.1f);
// 		face.Tangent = vec3(0.0f);
// 		face.Bitangent = vec3(0.0f);

// 		geometry.addVertex(face);
// 	}

// 	geometry.fillIndices();

// 	return geometry;
// }

Geometry Geometry::Box() {
	Geometry geometry;

	
	float vertices[] {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
	
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
	
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
	
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
	
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
	
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	const int verticesCount = 36;

	geometry.freeVerties();
	geometry.allocVertices(verticesCount);

	for (int i = 0; i < verticesCount; i++) {
		Vertex vertex(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
		geometry.addVertex(vertex);
	}

	console::info("Box", geometry.getVertices()->size());

	return geometry;
}
