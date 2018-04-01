//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_RENDER_SETUP_GEOMETRY_H
#define ACTIO_RENDER_SETUP_GEOMETRY_H

#include <entityx/entityx/Entity.h>
#include "../../core/mesh.h"

namespace game {
    namespace events {
        using namespace entityx;

        struct RenderSetupMesh {
            RenderSetupMesh(entityx::Entity entity, Mesh *mesh)
                    : entity(entity)
                    , mesh(mesh) {}

            entityx::Entity entity;
            Mesh *mesh;
        };
    }
}

#endif //ACTIO_RENDER_SETUP_GEOMETRY_H
