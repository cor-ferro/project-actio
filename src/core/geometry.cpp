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
	VAO = other.VAO;
	VBO = other.VBO;
	EBO = other.EBO;
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

		vertex.Position = libAi::toNativeType(mesh->mVertices[i]);
		vertex.Normal = libAi::toNativeType(mesh->mNormals[i]);

		if (mesh->mTextureCoords[0]) {
			vertex.TexCoords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		} else {			
			vertex.TexCoords = vec2(0.0f, 0.0f);
		}

		if (mesh->mTangents != nullptr) {
			vertex.Tangent = libAi::toNativeType(mesh->mTangents[i]);
			vertex.Bitangent = libAi::toNativeType(mesh->mBitangents[i]);
		} else {
			vertex.Tangent = vec3(1.0f);
			vertex.Bitangent = vec3(1.0f);
		}

		addVertex(vertex);
	}

	unsigned int numFaces = mesh->mNumFaces;
	freeIndices();
	allocIndices(numFaces);

	for (unsigned int i = 0; i < numFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices_->push_back(face.mIndices[j]);
		}
	}
}

void Geometry::setup()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	GeometryVertices * vertices = getVertices();

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &vertices->front(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	GeometryIndices * indices = getIndices();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(MeshIndex), &indices->front(), GL_STATIC_DRAW);

	setupVertex(vertices->front());

	glBindVertexArray(0);
}

void Geometry::setupVertex(Vertex& v)
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
				addIndex(i);
				continue;
			}

			Vertex& secondaryVertex = vertices_->at(j);

			if (glm::all(glm::epsilonEqual(primaryVertex.Position, secondaryVertex.Position, 0.01f)))
			{
				addIndex(i);
			}
			else
			{
				addIndex(j);
			}
		}
	}

	console::info("fillIndices: ", indices_->size());
}

void Geometry::addIndex(unsigned int i) {
	indices_->push_back(i);
}

void Geometry::addFace(unsigned int i1, unsigned int i2, unsigned int i3) {
	indices_->push_back(i1);
	indices_->push_back(i2);
	indices_->push_back(i3);
}

void Geometry::freeVerties() {
	SubTotalCountVertices(vertices_->size());
	vertices_->clear();
}

void Geometry::freeIndices() {
	indices_->clear();
}

Geometry Geometry::Box() {
	Geometry geometry;

	float vertices[] {
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
	};

	float normals[] {
		0.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, -0.0f, 0.0f,
		0.0f, -0.0f, 1.0f,
		-1.0f, -0.0f, -0.0f,
		0.0f, 0.0f, -1.0f
	};
	
	uint indices[] {
		1, 3, 0,
		7, 5, 4,
		4, 1, 0,
		5, 2, 1,
		2, 7, 3,
		0, 7, 4,
		1, 2, 3,
		7, 6, 5,
		4, 5, 1,
		5, 6, 2,
		2, 6, 7,
		0, 3, 7
	};

	const int verticesCount = 24;
	const int normalsCount = 18;
	const int indicesCount = 33;

	geometry.allocVertices(verticesCount);
	geometry.allocIndices(indicesCount);

	for (int i = 0; i <= verticesCount; i+=3) {
		Vertex vertex;
		vertex.Position = vec3(vertices[i], vertices[i+1], vertices[i+2]);
		geometry.addVertex(vertex);
	}

	for (int i = 0; i <= indicesCount; i+=3) {
		geometry.addFace(indices[i], indices[i+1], indices[i+2]);
	}

	return geometry;
}

Geometry Geometry::Plane(uint width, uint height, uint widthSegments, uint heightSegments) {
	Geometry geometry;

	float width_half = (float)width / 2.0f;
	float height_half = (float)height / 2.0f;

	uint gridX = widthSegments;
	uint gridY = heightSegments;

	uint gridX1 = gridX + 1;
	uint gridY1 = gridY + 1;

	float segment_width = (float)width / (float)gridX;
	float segment_height = (float)height / (float)gridY;

	geometry.allocVertices(gridY1 * gridX1);
	geometry.allocIndices(gridY * gridX);

	for (uint iy = 0; iy < gridY1; iy++) {
		float y = (float)iy * segment_height - height_half;

		for (uint ix = 0; ix < gridX1; ix++) {
			float x = (float)ix * segment_width - width_half;

			Vertex vertex;
			vertex.Position = vec3(x, -y, 0.0f);
			vertex.Normal = vec3(0.0f, 0.0f, 1.0f);

			geometry.addVertex(vertex);
		}
	}

	for (uint iy = 0; iy < gridY; iy++) {
		for (uint ix = 0; ix < gridX; ix++) {
			uint a = ix + gridX1 * iy;
			uint b = ix + gridX1 * (iy + 1);
			uint c = (ix + 1) + gridX1 * (iy + 1);
			uint d = (ix + 1) + gridX1 * iy;

			geometry.addFace(a, b, d);
			geometry.addFace(b, c, d);
		}
	}

	return geometry;
}

Geometry Geometry::Sphere(float radius,	uint widthSegments,	uint heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) {
	Geometry geometry;
	
	uint minWidthSegments = 3;
	uint minHeightSegments = 2;

	widthSegments = glm::max(minWidthSegments, widthSegments);
	heightSegments = glm::max(minHeightSegments, heightSegments);

	float thetaEnd = thetaStart + thetaLength;

	uint index = 0;
	std::vector<std::vector<uint>> grid;

	for (uint iy = 0; iy <= heightSegments; iy ++ ) {
		std::vector<uint> verticesRow;

		float v = (float)iy / (float)heightSegments;
		for (uint ix = 0; ix <= widthSegments; ix ++ ) {
			float u = (float)ix / (float)widthSegments;

			Vertex vertex;

			vertex.Position = vec3(
				-radius * glm::cos(phiStart + u * phiLength) * glm::sin(thetaStart + v * thetaLength),
				radius * glm::cos(thetaStart + v * thetaLength),
				radius * glm::sin(phiStart + u * phiLength) * glm::sin(thetaStart + v * thetaLength)
			);

			vertex.Normal = glm::normalize(vec3(
				vertex.Position.x,
				vertex.Position.y,
				vertex.Position.z
			));

			geometry.addVertex(vertex);

			verticesRow.push_back(index++);
		}

		grid.push_back(verticesRow);
	}

	// indices
	for (uint iy = 0; iy < heightSegments; iy++) {
		for (uint ix = 0; ix < widthSegments; ix++) {
			uint a = grid[iy][ix + 1];
			uint b = grid[iy][ix];
			uint c = grid[iy + 1][ix];
			uint d = grid[iy + 1][ix + 1];

			if ( iy != 0 || thetaStart > 0 ) {
				geometry.addFace(a, b, d);
			}

			if ( iy != heightSegments - 1 || thetaEnd < glm::pi<float>() ) {
				geometry.addFace(b, c, d);
			}
		}
	}

	return geometry;
}

Geometry Geometry::Circle(float radius,	uint segments, float thetaStart, float thetaLength)
{
	Geometry geometry;

	uint minSegments = 3;
	segments = glm::max(minSegments, segments);

	/*thetaStart = thetaStart !== undefined ? thetaStart : 0;
	thetaLength = thetaLength !== undefined ? thetaLength : Math.PI * 2;*/

	Vertex centerVertex;
	centerVertex.Position = vec3(0.0f, 0.0f, 0.0f);
	centerVertex.Normal = vec3(0.0f, 0.0f, 1.0f);

	geometry.addVertex(centerVertex);

	for (uint s = 0, i = 3; s <= segments; s++, i+= 3) {
		float segment = thetaStart + (float)s / (float)segments * thetaLength;

		Vertex vertex;
		vertex.Position = vec3(
			radius * glm::cos(segment),
			radius * glm::sin(segment),
			0.0f
		);
		vertex.Normal = vec3(
			0.0f,
			0.0f,
			1.0f
		);

		geometry.addVertex(vertex);
	}

	for (uint i = 1; i <= segments; i ++ ) {
		geometry.addFace(i, i + 1, 0);
	}

	return geometry;
}

void Geometry::CylinderTorso(GeometryCone& params, Geometry& geometry)
{
	std::vector<std::vector<uint>> indexArray;
	float halfHeight = params.height / 2.0f;
	float slope = (params.radiusBottom - params.radiusTop) / params.height;

	uint index = geometry.vertices_->size();

	for (uint y = 0; y <= params.heightSegments; y ++ ) {
		std::vector<uint> indexRow;

		float v = (float)y / (float)params.heightSegments;
		float radius = v * (params.radiusBottom - params.radiusTop) + params.radiusTop;

		for (uint x = 0; x <= params.radialSegments; x ++ ) {
			float u = (float)x / (float)params.radialSegments;
			float theta = u * params.thetaLength + params.thetaStart;

			float sinTheta = glm::sin(theta);
			float cosTheta = glm::cos(theta);

			// vertex
			Vertex vertex;
			vertex.Position = vec3(
				radius * sinTheta,
				-v * params.height + halfHeight,
				radius * cosTheta
			);
			vertex.Normal = glm::normalize(vec3(
				sinTheta,
				slope,
				cosTheta
			));

			geometry.addVertex(vertex);
			indexRow.push_back(index++);
		}

		indexArray.push_back(indexRow);
	}

	for (uint x = 0; x < params.radialSegments; x++) {
		for (uint y = 0; y < params.heightSegments; y++) {
			uint a = indexArray[y][x];
			uint b = indexArray[y + 1][x];
			uint c = indexArray[y + 1][x + 1];
			uint d = indexArray[y][x + 1];

			geometry.addFace(a, b, d);
			geometry.addFace(b, c, d);
		}
	}
}

void Geometry::CylinderCap(GeometryCone& params, Geometry& geometry, bool top)
{
	uint centerIndexStart, centerIndexEnd;

	float halfHeight = params.height / 2.0f;
	float radius = ( top == true ) ? params.radiusTop : params.radiusBottom;
	float sign = ( top == true ) ? 1.0 : -1.0;
	
	uint index = geometry.vertices_->size();

	centerIndexStart = index;

	for (uint x = 1; x <= params.radialSegments; x++) {
		Vertex vertex;
		vertex.Position = vec3(0.0f, halfHeight * sign, 0.0f);
		vertex.Normal = vec3(0.0f, sign, 0.0f);

		geometry.addVertex(vertex);

		index++;
	}

	centerIndexEnd = index;

	for (uint x = 0; x <= params.radialSegments; x ++ ) {
		float u = (float)x / (float)params.radialSegments;
		float theta = u * params.thetaLength + params.thetaStart;

		float cosTheta = glm::cos(theta);
		float sinTheta = glm::sin(theta);

		Vertex vertex;
		vertex.Position = vec3(radius * sinTheta, halfHeight * sign, radius * cosTheta);
		vertex.Normal = vec3(0.0f, sign, 0.0f);

		geometry.addVertex(vertex);

		index++;
	}

	for (uint x = 0; x < params.radialSegments; x ++ ) {
		uint c = centerIndexStart + x;
		uint i = centerIndexEnd + x;

		if ( top == true ) {
			geometry.addFace(i, i + 1, c);
		} else {
			geometry.addFace(i + 1, i, c);
		}
	}
}

Geometry Geometry::Cylinder(float radiusTop, float radiusBottom, float height, uint radialSegments, uint heightSegments, bool openEnded, float thetaStart, float thetaLength)
{
	Geometry geometry;
	GeometryCone params = {
		.radiusTop = radiusTop,
		.radiusBottom = radiusBottom,
		.height = height,
		.radialSegments = radialSegments,
		.heightSegments = heightSegments,
		.openEnded = openEnded,
		.thetaStart = thetaStart,
		.thetaLength = thetaLength
	};

	CylinderTorso(params, geometry);

	if ( openEnded == false ) {
		if ( radiusTop > 0 ) CylinderCap(params, geometry, true);
		if ( radiusBottom > 0 ) CylinderCap(params, geometry, false);
	}

	return geometry;
}

Geometry Geometry::Cone(float radius, float height, uint radialSegments, uint heightSegments, bool openEnded, float thetaStart, float thetaLength)
{
	return Geometry::Cylinder(0.0f, radius, height, radialSegments, heightSegments, openEnded, thetaStart, thetaLength);
}

Geometry Geometry::Ring(float innerRadius, float outerRadius, uint thetaSegments, uint phiSegments, float thetaStart, float thetaLength)
{
	Geometry geometry;

	float radius = innerRadius;
	float radiusStep = ((outerRadius - innerRadius) / (float)phiSegments);

	for (uint j = 0; j <= phiSegments; j ++ ) {
		for (uint i = 0; i <= thetaSegments; i ++ ) {
			float segment = thetaStart + (float)i / (float)thetaSegments * thetaLength;

			Vertex vertex;
			vertex.Position = vec3(
				radius * glm::cos(segment),
				radius * glm::sin(segment),
				0.0f
			);
			vertex.Normal = vec3(
				0.0f,
				0.0f,
				1.0f
			);
			
			geometry.addVertex(vertex);
		}

		radius += radiusStep;
	}

	for (uint j = 0; j < phiSegments; j ++ ) {
		uint thetaSegmentLevel = j * (thetaSegments + 1);
		for (uint i = 0; i < thetaSegments; i ++ ) {
			uint segment = i + thetaSegmentLevel;

			uint a = segment;
			uint b = segment + thetaSegments + 1;
			uint c = segment + thetaSegments + 2;
			uint d = segment + 1;

			geometry.addFace(a, b, d);
			geometry.addFace(b, c, d);
		}
	}

	return geometry;
}

Geometry Geometry::Torus(float radius, float tube, uint radialSegments, uint tubularSegments, float arc)
{
	Geometry geometry;

	// generate vertices, normals and uvs
	for (uint j = 0; j <= radialSegments; j ++ ) {
		for (uint i = 0; i <= tubularSegments; i ++ ) {

			float u = (float)i / (float)tubularSegments * arc;
			float v = (float)j / (float)radialSegments * glm::two_pi<float>();

			Vertex vertex;
			vertex.Position = vec3(
				(radius + tube * glm::cos(v)) * glm::cos(u),
				(radius + tube * glm::cos(v)) * glm::sin(u),
				tube * glm::sin( v )
			);

			vec3 center(
				radius * glm::cos(u),
				radius * glm::sin(u),
				0.0f
			);

			vertex.Normal = glm::normalize(center - vertex.Position);

			geometry.addVertex(vertex);
		}
	}

	for (uint j = 1; j <= radialSegments; j ++ ) {
		for (uint i = 1; i <= tubularSegments; i ++ ) {
			uint a = (tubularSegments + 1) * j + i - 1;
			uint b = (tubularSegments + 1) * (j - 1) + i - 1;
			uint c = (tubularSegments + 1) * (j - 1) + i;
			uint d = (tubularSegments + 1) * j + i;

			geometry.addFace(a, b, d);
			geometry.addFace(b, c, d);
		}
	}

	return geometry;
}

Geometry Geometry::Octahedron(float radius)
{
	Geometry geometry;

	float vertices[] = {
		1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, -1.0f
	};

	uint indices[] = {
		0, 2, 4,
		0, 4, 3,
		0, 3, 5,
		0, 5, 2,
		1, 2, 5,
		1, 5, 3,
		1, 3, 4,
		1, 4, 2
	};

	uint countVertices = 18;
	uint countIndices = 24;

	for (uint i = 0; i < countVertices; i+= 3) {
		Vertex vertex;
		vertex.Position = vec3(vertices[i] * radius, vertices[i+1] * radius, vertices[i+2] * radius);
		vertex.Normal = glm::normalize(vec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));

		geometry.addVertex(vertex);
	}

	for (uint i = 0; i < countIndices; i+= 3) {
		geometry.addFace(indices[i], indices[i+1], indices[i+2]);
	}

	return geometry;
}

Geometry Geometry::Quad2d()
{
	Geometry geometry;

	float vertices[] = {
		1.0f,  1.0f, 0.0f,
     	1.0f, -1.0f, 0.0f,
    	-1.0f, -1.0f, 0.0f,
    	-1.0f,  1.0f, 0.0f
	};

	uint indices[] = {
		0, 1, 3,
    	1, 2, 3
	};

	uint countVertices = 12;
	uint countIndices = 6;

	for (uint i = 0; i < countVertices; i+= 3) {
		Vertex vertex;
		vertex.Position = vec3(vertices[i], vertices[i+1], vertices[i+2]);
		vertex.Normal = glm::normalize(vec3(vertex.Position.x, vertex.Position.y, vertex.Position.z));

		geometry.addVertex(vertex);
	}

	for (uint i = 0; i < countIndices; i+= 3) {
		geometry.addFace(indices[i], indices[i+1], indices[i+2]);
	}

	return geometry;
}

void Geometry::computeBoundingBox()
{
	vec3 min;
	vec3 max;

	const Vertex& frontVertex = vertices_->front();

	min.x = max.x = frontVertex.Position.x;
	min.y = max.y = frontVertex.Position.y;
	min.z = max.z = frontVertex.Position.z;
	
	for (auto verticesIt = vertices_->begin(); verticesIt != vertices_->end(); verticesIt++)
	{
		Vertex& vertex = *verticesIt;
		
		if (vertex.Position.x < min.x) min.x = vertex.Position.x;
		if (vertex.Position.x > max.x) max.x = vertex.Position.x;
		if (vertex.Position.y < min.y) min.y = vertex.Position.y;
		if (vertex.Position.y > max.y) max.y = vertex.Position.y;
		if (vertex.Position.z < min.z) min.z = vertex.Position.z;
		if (vertex.Position.z > max.z) max.z = vertex.Position.z;
	}

	boundingBox.center.x = (min.x + max.x) / 2.0f;
	boundingBox.center.y = (min.y + max.y) / 2.0f;
	boundingBox.center.z = (min.z + max.z) / 2.0f;

	boundingBox.radius.x = (max.x - min.x) / 2.0f;
	boundingBox.radius.y = (max.y - min.y) / 2.0f;
	boundingBox.radius.z = (max.z - min.z) / 2.0f;

	vec3 size = vec3(max.x - min.x, max.y - min.y, max.z - min.z);
	vec3 center = vec3((min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f, (min.z + max.z) / 2.0f);
	//mat4 transform = glm::translate(glm::mat4(1.0f), center) * glm::scale(glm::mat4(1.0f), size);

	console::info("");
	console::info("bounding box");
	console::info("size: ", size.x, ", ", size.y, ", ", size.z);
	console::info("center: ", center.x, ", ", center.y, ", ", center.z);
	console::info("");
}

void Geometry::computeBoundingSphere()
{
	
}

const Math::Box3& Geometry::getBoundingBox()
{
	return boundingBox;
}
