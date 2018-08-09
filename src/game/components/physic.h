#ifndef GAME_COMPONENTS_PHYSIC_H_
#define GAME_COMPONENTS_PHYSIC_H_

#include <glm/glm.hpp>
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxMaterial.h"

namespace game {
    namespace components {
        using namespace physx;

        struct Physic {
            explicit Physic(PxRigidDynamic *dynamic) :  dynamic(dynamic) {};

            ~Physic() {
                if (dynamic != nullptr) {
                    dynamic->userData = nullptr;

                    auto *scene = dynamic->getScene();

                    if (scene == nullptr) return;

                    scene->removeActor(*dynamic);
                }
            }

            PxRigidDynamic *dynamic = nullptr;
        };
    }
}

#endif
