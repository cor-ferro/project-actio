#ifndef GAME_COMPONENTS_PHYSIC_H_
#define GAME_COMPONENTS_PHYSIC_H_

#include <glm/glm.hpp>
#include "PxRigidDynamic.h"
#include "PxMaterial.h"

namespace game {
    namespace components {
		using namespace physx;

        struct Physic {
        	Physic() {
        		material = nullptr;
        		dynamic = nullptr;
            };

			PxMaterial* material;
			PxRigidDynamic* dynamic;
        };
    }
}

#endif
