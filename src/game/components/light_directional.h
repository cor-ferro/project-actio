//
// Created by demitriy on 3/29/18.
//

#ifndef ACTIO_LIGHT_DIRECTIONAL_H
#define ACTIO_LIGHT_DIRECTIONAL_H

#include "../../lights/direction_light.h"

namespace game {
    namespace components {
        struct LightDirectional : Light::Directional {
            LightDirectional(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction)
                    : Light::Directional(ambient, diffuse, specular, direction) {};

            LightDirectional()
                    : Light::Directional() {}

        };
    }
}

#endif //ACTIO_LIGHT_DIRECTIONAL_H
