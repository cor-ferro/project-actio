//
// Created by demitriy on 7/28/18.
//

#ifndef ACTIO_EVENTS_RENDER_UPDATE_MESH_H
#define ACTIO_EVENTS_RENDER_UPDATE_MESH_H

#include <entityx/entityx/Entity.h>
#include "../world_object.h"
#include "../../core/model.h"

namespace game {
    namespace events {
        namespace ex = entityx;

        struct RenderUpdateMesh {
            explicit RenderUpdateMesh(ex::Entity& entity) : entity(entity) {};

            ex::Entity entity;
        };
    }
}

#endif //ACTIO_EVENTS_RENDER_UPDATE_MESH_H
