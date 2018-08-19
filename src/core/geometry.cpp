#include "geometry.h"

Geometry::Geometry() {
    vertices_.reset(new GeometryVertices());
    indices_.reset(new GeometryIndices());

    allocVertices(1);
    allocIndices(1);
}

Geometry::Geometry(const Geometry &other) {
    vertices_ = other.vertices_;
    indices_ = other.indices_;
    type = other.type;
    console::info("copy geometry %i %i", other.type, type);
}

Geometry::~Geometry() {
}

void Geometry::destroy() {
    freeVerties();
    freeIndices();
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

void Geometry::addVertex(const Vertex &vertex) {
    vertices_->push_back(vertex);
}

void Geometry::addVertex(const Vertex &&vertex) {
    vertices_->push_back(vertex);
}

void Geometry::addVertex(float x, float y, float z) {
    Vertex v(x, y, z);
    vertices_->push_back(v);
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
    vertices_->clear();
}

void Geometry::freeIndices() {
    indices_->clear();
}

void Geometry::computeBoundingBox() {
    vec3 min(0.0f);
    vec3 max(0.0f);

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
}

void Geometry::computeBoundingSphere() {

}

const Math::Box3 &Geometry::getBoundingBox() {
    return boundingBox;
}

float Geometry::height() {
    return boundingBox.radius.y * 2.0f;
}

float Geometry::halfHeight() {
    return boundingBox.radius.y;
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
