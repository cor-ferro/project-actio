#include "geometry.h"

#define MAX_BONES 4

Geometry::Geometry() {
    vertices_.reset(new GeometryVertices());
    indices_.reset(new GeometryIndices());

    allocVertices(1);
    allocIndices(1);
}

Geometry::Geometry(aiMesh *mesh, const Resource::Assimp *assimpResource) {
    vertices_.reset(new GeometryVertices());
    indices_.reset(new GeometryIndices());

    initFromAi(mesh, assimpResource);
}

Geometry::Geometry(const Geometry &other) {
    vertices_ = other.vertices_;
    indices_ = other.indices_;
    VAO = other.VAO;
    VBO = other.VBO;
    EBO = other.EBO;
    type = other.type;
    console::info("copy geometry %i %i", other.type, type);
}

Geometry::~Geometry() {
}

void Geometry::destroy() {
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);

    freeVerties();
    freeIndices();
}

void Geometry::initFromAi(const aiMesh *mesh, const Resource::Assimp *assimpResource) {
    unsigned int numVertices = mesh->mNumVertices;
    unsigned int numBones = mesh->mNumBones;
    allocVertices(numVertices);

    GetTotalCountVertices(); // why?

    for (unsigned int i = 0; i < numVertices; i++) {
        Vertex vertex;

        vertex.Position = libAi::toNativeType(mesh->mVertices[i]);

        if (mesh->mNormals != nullptr) {
            vertex.Normal = libAi::toNativeType(mesh->mNormals[i]);
        } else {
            vertex.Normal = vec3(0.0f, 1.0f, 0.0f);
        }

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

    if (numBones > 0) {
        std::vector<std::vector<std::pair<uint, float>>> affectedVertices;
        affectedVertices.resize(vertices_->size());

        for (uint i = 0; i < numBones; i++) {
            aiBone *bone = mesh->mBones[i];

            for (uint j = 0; j < bone->mNumWeights; j++) {
                uint vertexId = bone->mWeights[j].mVertexId;
                float weight = bone->mWeights[j].mWeight;

                affectedVertices.at(vertexId).push_back({i, weight});
            }
        }

        for (uint i = 0; i < affectedVertices.size(); i++) {
            const std::vector<std::pair<uint, float>> &boneData = affectedVertices.at(i);
            const int bonesDataSize = boneData.size();

            for (uint j = 0; j < MAX_BONES; j++) {
                if (j < bonesDataSize) {
                    vertices_->at(i).BonedIDs[j] = boneData[j].first;
                    vertices_->at(i).Weights[j] = boneData[j].second;
                } else {
                    vertices_->at(i).BonedIDs[j] = 0;
                    vertices_->at(i).Weights[j] = 0;
                }
            }
        }
    }

    unsigned int numFaces = mesh->mNumFaces;
    freeIndices();
    allocIndices(numFaces);

    for (unsigned int i = 0; i < numFaces; i++) {
        aiFace &face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices_->push_back(face.mIndices[j]);
        }
    }
}

void Geometry::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    GeometryVertices *vertices = getVertices();

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(Vertex), &vertices->front(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    GeometryIndices *indices = getIndices();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size() * sizeof(MeshIndex), &indices->front(), GL_STATIC_DRAW);

    setupVertex(vertices->front());

    glBindVertexArray(0);
}

void Geometry::setupVertex(Vertex &v) {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Bitangent));

    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *) offsetof(Vertex, BonedIDs));

    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Weights));
}

GeometryVertices *Geometry::getVertices() {
    return vertices_.get();
}

GeometryIndices *Geometry::getIndices() {
    return indices_.get();
}

void Geometry::setType(Geometry::GeometryType newType) {
    type = newType;
}

Geometry::GeometryType Geometry::getType() {
    return type;
}

void Geometry::setVertices(std::vector<vec3> vertices) {
    freeVerties();
    allocVertices(vertices.size());

    for (vec3 &vertex : vertices) {
        addVertex(vertex.x, vertex.y, vertex.z);
    }
}

void Geometry::addVertex(Vertex vertex) {
    vertices_->push_back(vertex);
    AddTotalCountVertices(1);
}

void Geometry::addVertex(float x, float y, float z) {
    Vertex v(x, y, z);
    vertices_->push_back(v);
    AddTotalCountVertices(1);
}

void Geometry::addVertex(std::vector<float> &vertices) {
    int verticesSize = vertices.size();
    for (int i = 0; i < verticesSize; i += 3) {
        addVertex(vertices[i], vertices[i + 1], vertices[i + 2]);
    }
}

Vertex &Geometry::getVertex(unsigned int index) {
    return vertices_->at(index);
}

void Geometry::allocVertices(unsigned int count) {
    vertices_->reserve(count);
}

void Geometry::allocIndices(unsigned int count) {
    indices_->reserve(count);
}

void Geometry::fillIndices() {
    allocIndices(vertices_->size());
    for (int i = 0; i < vertices_->size(); i++) {
        Vertex &primaryVertex = vertices_->at(i);

        for (int j = 0; j < vertices_->size(); j++) {
            if (j == i) {
                addIndex(i);
                continue;
            }

            Vertex &secondaryVertex = vertices_->at(j);

            if (glm::all(glm::epsilonEqual(primaryVertex.Position, secondaryVertex.Position, 0.01f))) {
                addIndex(i);
            } else {
                addIndex(j);
            }
        }
    }

    console::info("fillIndices: %i", indices_->size());
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

void Geometry::computeBoundingBox() {
    vec3 min;
    vec3 max;

    const Vertex &frontVertex = vertices_->front();

    min.x = max.x = frontVertex.Position.x;
    min.y = max.y = frontVertex.Position.y;
    min.z = max.z = frontVertex.Position.z;

    for (auto verticesIt = vertices_->begin(); verticesIt != vertices_->end(); verticesIt++) {
        Vertex &vertex = *verticesIt;

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

    console::info("bounding box");
    console::info("size: %i %i %i", size.x, size.y, size.z);
    console::info("center: %i %i %i", center.x, center.y, center.z);
}

void Geometry::computeBoundingSphere() {

}

const Math::Box3 &Geometry::getBoundingBox() {
    return boundingBox;
}

size_t Geometry::getCountVertices() {
    return vertices_->size();
};

size_t Geometry::getCountIndices() {
    return indices_->size();
};

void Geometry::setNeedUpdateVertices(bool value) {
    needUpdateVertices = value;
};

void Geometry::setNeedUpdateIndices(bool value) {
    needUpdateIndices = value;
};

bool Geometry::isNeedUpdateVertices() {
    return needUpdateVertices;
};

bool Geometry::isNeedUpdateIndices() {
    return needUpdateIndices;
};
