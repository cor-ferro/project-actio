//
// Created by demitriy on 3/31/18.
//

#ifndef ACTIO_RENDER_SETUP_GEOMETRY_H
#define ACTIO_RENDER_SETUP_GEOMETRY_H

#include <entityx/entityx/Entity.h>
#include "../../core/mesh.h"

namespace game {
    namespace events {
        namespace ex = entityx;

        struct RenderSetupMesh {
            RenderSetupMesh(ex::Entity entity, Mesh *mesh)
                    : entity(entity)
                    , mesh(mesh) {}

            ex::Entity entity;
            Mesh *mesh;
        };
    }
}

#endif //ACTIO_RENDER_SETUP_GEOMETRY_H
