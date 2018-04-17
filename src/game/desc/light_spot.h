#ifndef ACTIO_LIGHT_SPOT_H
#define ACTIO_LIGHT_SPOT_H

#include <entityx/entityx/entityx.h>
#include "../../lib/types.h"

namespace game {
    namespace desc {
        using namespace entityx;

        struct LightSpotDesc {
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec3 position;
            vec3 direction;
            float constant;
            float linear;
            float quadratic;
            float cutOff;
            float outerCutOff;
        };
    }
}

#endif //ACTIO_LIGHT_POINT_H