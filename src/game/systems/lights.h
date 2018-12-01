//
// Created by demitriy on 11/7/18.
//

#ifndef ACTIO_GAME_SYSTEMS_LIGHTS_H
#define ACTIO_GAME_SYSTEMS_LIGHTS_H

#include <entityx/entityx.h>
#include "base.h"
#include "../desc/light_point.h"
#include "../desc/light_directional.h"
#include "../desc/light_spot.h"

namespace game {
    namespace systems {
        namespace ex = entityx;

        class Lights : public systems::BaseSystem, public entityx::System<Lights>, public entityx::Receiver<Lights> {
        public:
            Lights(Context& context);

            void update(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt);

            void createLight(const desc::LightPointDesc& description, ex::Entity &entity);

            void createLight(const desc::LightDirectionalDesc& description, ex::Entity &entity);

            void createLight(const desc::LightSpotDesc& description, ex::Entity &entity);
        };
    }
}

#endif //ACTIO_GAME_SYSTEMS_LIGHTS_H
