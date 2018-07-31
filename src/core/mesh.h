#ifndef ACTIO_CORE_MESH_H_
#define ACTIO_CORE_MESH_H_

#include <vector>
#include <unordered_map>
#include <string>
#include "material.h"
#include "../resources/resources.h"
#include "../memory/poolallocator.h"
#include "geometry.h"
#include "texture.h"
#include "mesh_bone.h"
#include "object3D.h"
#include "vertex.h"

extern memory::PoolAllocator *meshAllocator;

typedef std::size_t MeshId;

static size_t idMeshCounter = 0;

static MeshId newMeshId() {
    return ++idMeshCounter;
}

enum MeshDrawType {
    Mesh_Draw_Triangle,
    Mesh_Draw_Line,
    Mesh_Draw_Line_Loop,
    Mesh_Draw_Point
};

enum MeshDrawMode {
    Mesh_Draw_Arrays,
    Mesh_Draw_Elements
};

enum MeshDrawFlags {
    Mesh_Draw_Base = 0x1,
    Mesh_Draw_Textures = 0x2,
    Mesh_Draw_Bones = 0x4,
    Mesh_Draw_Material = 0x8,
    Mesh_Draw_All = Mesh_Draw_Base | Mesh_Draw_Textures | Mesh_Draw_Bones | Mesh_Draw_Material
};

struct Mesh : Object3D {
    struct BonesMap {
        unsigned int count;
        std::vector<mat4> transforms;
        std::vector<mat4> offsets;
        std::vector<unsigned int> indexes;
        std::vector<std::string> names;

        void resize(unsigned int size) {
            transforms.resize(size);
            offsets.resize(size);
            indexes.resize(size);
            names.resize(size);
            count = size;
        }
    };

    static Mesh *Create();

    static Mesh *Create(std::shared_ptr<Material> &material);

    static void Destroy(Mesh *mesh);

    ~Mesh();

    std::string getName();

    MeshId getId();

    void setName(std::string newName);

    void setName(const char *newName);

    void setDrawType(MeshDrawType type);

    MeshDrawType getDrawType();

    const Geometry &getGeometry() const;

    Geometry &getGeometry();

    std::shared_ptr<Material> material;
    Geometry geometry;
    BonesMap bones;
private:
    Mesh();

    Mesh(const Mesh &mesh);

    void destroy();

    std::size_t id;
    std::string name;
    MeshDrawType drawType;
};

#endif
