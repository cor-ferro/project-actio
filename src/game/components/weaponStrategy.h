#ifndef ACTIO_COMPONENTS_WEAPON_STRATEGY_H
#define ACTIO_COMPONENTS_WEAPON_STRATEGY_H

#include <assert.h>
#include <string>
#include <glm/glm/common.hpp>
#include "../../lib/console.h"
#include "../../lib/types.h"
#include "../desc/weapon.h"
#include "../weapon_handler.h"
#include "../strategies/weapons/base.h"

namespace game {
    namespace components {
        struct WeaponStrategy {
            explicit WeaponStrategy(strategy::WeaponsBase *strategy) : handler(strategy) {}

            strategy::WeaponsBase *handler = nullptr;
        };
    }
}

#endif //ACTIO_COMPONENTS_WEAPON_STRATEGY_H
