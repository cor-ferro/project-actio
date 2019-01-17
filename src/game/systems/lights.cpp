#include "lights.h"
#include "../components/light_spot.h"
#include "../components/light_point.h"
#include "../components/light_directional.h"
#include "../components/transform.h"

namespace game {
    namespace systems {
        Lights::Lights(Context& context) : systems::BaseSystem(context) {}

        void Lights::update(ex::EntityManager& es, ex::EventManager& events, ex::TimeDelta dt) {

        }

        void Lights::createLight(const desc::LightPointDesc& lightDescription, ex::Entity &entity) {
            components::LightPoint light;
            light.setAmbient(lightDescription.ambient);
            light.setDiffuse(lightDescription.diffuse);
            light.setSpecular(lightDescription.specular);
            light.setPosition(lightDescription.position);
            light.setAttenuation(lightDescription.constant, lightDescription.linear, lightDescription.quadratic);

            entity.assign<components::LightPoint>(light);
            entity.assign<components::Transform>(lightDescription.position);
        }

        void Lights::createLight(const desc::LightDirectionalDesc& lightDescription, ex::Entity &entity) {
            components::LightDirectional light;
            light.setAmbient(lightDescription.ambient);
            light.setDiffuse(lightDescription.diffuse);
            light.setSpecular(lightDescription.specular);
            light.setDirection(lightDescription.direction);

            entity.assign<components::LightDirectional>(light);
            entity.assign<components::Transform>(lightDescription.position);
        }

        void Lights::createLight(const desc::LightSpotDesc& lightDescription, ex::Entity &entity) {
            game::components::LightSpot light;
            light.setAmbient(lightDescription.ambient);
            light.setDiffuse(lightDescription.diffuse);
            light.setSpecular(lightDescription.specular);
            light.setPosition(lightDescription.position);
            light.setDirection(lightDescription.direction);
            light.setAttenuation(lightDescription.constant, lightDescription.linear, lightDescription.quadratic);
            light.setCutoff(lightDescription.cutOff, lightDescription.outerCutOff);

            entity.assign<game::components::LightSpot>(light);
            entity.assign<game::components::Transform>(lightDescription.position);
        }
    }
}

