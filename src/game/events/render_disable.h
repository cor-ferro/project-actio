//
// Created by demitriy on 4/8/18.
//

#ifndef ACTIO_RENDER_DISABLE_H
#define ACTIO_RENDER_DISABLE_H

#include <entityx/entityx/Entity.h>

namespace game {
    namespace events {
        struct RenderDisable {
            explicit RenderDisable(entityx::Entity entity) : entity(entity) {}

            entityx::Entity entity;
        };
    }
}

#endif //ACTIO_RENDER_DISABLE_H
