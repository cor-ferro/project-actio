//
// Created by demitriy on 7/28/18.
//

#ifndef ACTIO_EVENTS_RENDER_SETUP_MODEL_H
#define ACTIO_EVENTS_RENDER_SETUP_MODEL_H

#include <entityx/entityx/Entity.h>
#include "../world_object.h"
#include "../../core/model.h"

namespace game {
    namespace events {
        namespace ex = entityx;

        struct RenderSetupModel {
            enum class Action {
                Add = 1,
                Remove = 2,
                Update = 3
            };

            explicit RenderSetupModel(ex::Entity entity, Action action = Action::Add)
                : entity(entity)
                , action(action) {};

            explicit RenderSetupModel(game::WorldObject *object, Action action = Action::Add)
                : entity(object->getEntity())
                , action(action) {};

            ex::Entity entity;
            Action action = Action::Add;
        };
    }
}

#endif //ACTIO_EVENTS_RENDER_SETUP_MODEL_H
