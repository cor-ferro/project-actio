//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_RENDER_UPDATE_MESH_H
#define ACTIO_RENDER_UPDATE_MESH_H

#include <entityx/entityx/Entity.h>
#include "../../core/mesh.h"

namespace game {
    namespace events {
        using namespace entityx;

        struct RenderUpdateMesh {
            RenderUpdateMesh(entityx::Entity entity, Mesh *mesh)
                    : entity(entity)
                    , mesh(mesh) {}

            entityx::Entity entity;
            Mesh *mesh;
        };
    }
}

#endif //ACTIO_RENDER_UPDATE_MESH_H
