//
// Created by demitriy on 4/17/18.
//

#ifndef ACTIO_LIGHT_POINT_H
#define ACTIO_LIGHT_POINT_H

#include "../../lib/types.h"

namespace game {
    namespace desc {
        struct LightPointDesc {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec3 position;
            vec3 direction;
            float constant;
            float linear;
            float quadratic;
        };
    }
}

#endif //ACTIO_LIGHT_POINT_H
