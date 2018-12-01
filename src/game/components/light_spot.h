//
// Created by demitriy on 3/29/18.
//

#ifndef ACTIO_GAME_LIGHT_SPOT_H
#define ACTIO_GAME_LIGHT_SPOT_H

#include "../../lights/spot_light.h"

namespace game {
    namespace components {
        struct LightSpot : Light::Spot {
            LightSpot() : Light::Spot() {};
        };
    }
}

#endif //ACTIO_LIGHT_SPOT_H
