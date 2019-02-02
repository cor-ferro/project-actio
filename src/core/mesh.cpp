#include <utility>
#include <glad/glad.h>
#include <glm/ext.hpp>
#include "../lib/colors.h"
#include "mesh.h"

Mesh::Mesh()
        : name("")
        , id(newMeshId())
        , primitive(Mesh_Primitive_Triangle)
{}

Mesh::~Mesh() {
    destroy();
}

void Mesh::destroy() {
    geometry.destroy();
}

const std::string& Mesh::getName() {
    return name;
}

MeshId Mesh::getId() {
    return id;
}

void Mesh::setName(const std::string& newName) {
    name = newName;
}

void Mesh::setPrimitiveType(MeshPrimitiveType type) {
    primitive = type;
}

MeshPrimitiveType Mesh::getPrimitiveType() {
    return primitive;
}

const Geometry &Mesh::getGeometry() const {
    return geometry;
}

Geometry &Mesh::getGeometry() {
    return geometry;
}
