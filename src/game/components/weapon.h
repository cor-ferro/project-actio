//
// Created by demitriy on 4/25/18.
//

#ifndef ACTIO_COMPONENTS_WEAPON_H
#define ACTIO_COMPONENTS_WEAPON_H

#include <assert.h>
#include <string>
#include <glm/glm/common.hpp>
#include "../../lib/console.h"
#include "../../lib/types.h"
#include "../desc/weapon.h"
#include "../weapon_handler.h"


namespace game {
    namespace components {
        struct Weapon : game::Weapon {
            explicit Weapon() = default;

            explicit Weapon(const desc::Weapon& description) : game::Weapon(description) {}
        };
    }
}

#endif //ACTIO_COMPONENTS_WEAPON_H
