//
// Created by demitriy on 4/5/18.
//

#ifndef ACTIO_ANIMATIONS_H
#define ACTIO_ANIMATIONS_H

#include <entityx/entityx/System.h>
#include <entityx/entityx/config.h>
#include <entityx/entityx/Entity.h>
#include <entityx/entityx/Event.h>
#include "../components/skin.h"
#include "../components/model.h"

namespace game {
    namespace systems {
        using namespace entityx;

        class Animations : public entityx::System<Animations> {
        public:
            void update(EntityManager &es, EventManager &events, TimeDelta dt) override {
                ComponentHandle<components::Model> model;
                ComponentHandle<components::Skin> skin;

                for (Entity entity : es.entities_with_components(model, skin)) {
                    if (skin->canProcessAnimation()) {
                        skin->processAnimation(dt);

                        mat4 *transforms = skin->getTransforms();

                        for (Mesh *mesh : model->getMeshes())
                        {
                            for (unsigned int boneIndex = 0; boneIndex < mesh->bones.count; boneIndex++) {
                                const unsigned int &nodeIndex = mesh->bones.indexes[boneIndex];

                                mesh->bones.transforms[boneIndex] = skin->inverseMat * transforms[nodeIndex];
                            }
                        }
                    }
                }
            }
        };
    }
}

#endif //ACTIO_ANIMATIONS_H