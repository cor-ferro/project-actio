//
// Created by demitriy on 4/8/18.
//

#ifndef ACTIO_BASE_H
#define ACTIO_BASE_H

#include <entityx/entityx/Entity.h>

#define GE

namespace game {
    namespace components {
        template <typename T>
        inline entityx::ComponentHandle<T> get(entityx::Entity &entity) {
            entityx::ComponentHandle<T> model = entity.component<T>();
        }
    }
}

#endif //ACTIO_BASE_H
