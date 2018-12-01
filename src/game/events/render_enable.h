//
// Created by demitriy on 4/8/18.
//

#ifndef ACTIO_RENDER_ENABLE_H
#define ACTIO_RENDER_ENABLE_H

#include <entityx/entityx/Entity.h>

namespace game {
    namespace events {
        struct RenderEnable {
            explicit RenderEnable(entityx::Entity entity) : entity(entity) {}

            entityx::Entity entity;
        };
    }
}

#endif //ACTIO_RENDER_ENABLE_H
