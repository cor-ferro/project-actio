#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/ext.hpp>
#include "../lib/assimp.h"
#include "../lib/colors.h"
#include "mesh.h"

Mesh::Mesh()
        : name("")
        , id(newMeshId())
        , drawType(Mesh_Draw_Triangle) {}

Mesh::Mesh(const Mesh &other) {
    console::info("copy mesh");

    name = other.name;
    material = other.material;
    geometry = other.geometry;
    drawType = other.drawType;
}

Mesh *Mesh::Create() {
    void *place = meshAllocator->Allocate(sizeof(Mesh), 8);
    Mesh *mesh = new(place) Mesh();
    mesh->material.reset(new Material());

    return mesh;
}

Mesh *Mesh::Create(std::shared_ptr<Material> &material) {
    void *place = meshAllocator->Allocate(sizeof(Mesh), 8);
    Mesh *mesh = new(place) Mesh();
    mesh->material = material;

    return mesh;
}

void Mesh::Destroy(Mesh *mesh) {
    if (mesh == nullptr) return;

    mesh->destroy();
    mesh->~Mesh();
    meshAllocator->Free((void *) mesh);
}

Mesh::~Mesh() {
	console::info("free mesh %i", id);
}

void Mesh::destroy() {
    geometry.destroy(); // @todo: удаление ресурсов не работает корректно, из за переноса логики setup в рендер
}

std::string Mesh::getName() {
    return name;
}

MeshId Mesh::getId() {
    return id;
}

void Mesh::setName(std::string newName) {
    name = newName;
}

void Mesh::setName(const char *newName) {
    name = std::string(newName);
}

void Mesh::setDrawType(MeshDrawType type) {
    drawType = type;
}

MeshDrawType Mesh::getDrawType() {
    return drawType;
}

const Geometry &Mesh::getGeometry() const {
    return geometry;
}

Geometry &Mesh::getGeometry() {
    return geometry;
}
