#ifndef GAME_COMPONENTS_PHYSIC_H_
#define GAME_COMPONENTS_PHYSIC_H_

#include <glm/glm.hpp>
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxMaterial.h"

namespace game {
    namespace components {
        namespace px = physx;

        struct Physic {
            enum Type {
                Character,
                Weapon,
                WeaponProjectile,
                StaticObject,
                DynamicObject,
                Other
            };

            explicit Physic(px::PxRigidActor *actor) :  actor(actor) {
                if (actor->is<px::PxRigidDynamic>()) {
                    type = DynamicObject;
                } else if (actor->is<px::PxRigidStatic>()) {
                    type = StaticObject;
                } else {
                    type = Other;
                }
            }

            explicit Physic(px::PxRigidActor *actor, Type type) : actor(actor), type(type) {}

            ~Physic() {
                if (actor != nullptr) {
                    actor->userData = nullptr;

                    auto *scene = actor->getScene();

                    if (scene == nullptr) {
                        return;
                    }

                    scene->removeActor(*actor);
                }
            }

            px::PxRigidActor *actor = nullptr;
            Type type = Other;
        };
    }
}

#endif
