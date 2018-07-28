//
// Created by demitriy on 4/17/18.
//

#ifndef ACTIO_LIGHT_DIRECTIONAL_H
#define ACTIO_LIGHT_DIRECTIONAL_H

#include "../../lib/types.h"

namespace game {
    namespace desc {
        struct LightDirectionalDesc {
            LightDirectionalDesc() = default;

            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec3 position;
            vec3 direction;

            void setAmbient(vec3 value) {  }
            void setDiffuse(vec3 value) {  }
            void setSpecular(vec3 value) {  }
            void setPosition(vec3 value) {  }
            void setDirection(vec3 value) {  }

            vec3 getAmbient() const { return ambient; }
            vec3 getDiffuse() const { return diffuse; }
            vec3 getSpecular() const { return specular; }
            vec3 getPosition() const { return position; }
            vec3 getDirection() const { return direction; }
        };
    }
}

#endif //ACTIO_LIGHT_DIRECTIONAL_H
