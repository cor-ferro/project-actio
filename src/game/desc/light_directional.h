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

            vec3 ambient = vec3(0.0f);
            vec3 diffuse = vec3(0.0f);
            vec3 specular = vec3(0.0f);
            vec3 position = vec3(0.0f);
            vec3 direction = vec3(0.0f);

            void setAmbient(const vec3 &value) { ambient = value; }
            void setDiffuse(const vec3 &value) { diffuse = value; }
            void setSpecular(const vec3 &value) { specular = value; }
            void setPosition(const vec3 &value) { position = value; }
            void setDirection(const vec3 &value) { direction = value; }

            vec3 getAmbient() const { return ambient; }
            vec3 getDiffuse() const { return diffuse; }
            vec3 getSpecular() const { return specular; }
            vec3 getPosition() const { return position; }
            vec3 getDirection() const { return direction; }
        };
    }
}

#endif //ACTIO_LIGHT_DIRECTIONAL_H
