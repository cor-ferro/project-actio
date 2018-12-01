//
// Created by demitriy on 4/17/18.
//

#ifndef ACTIO_LIGHT_POINT_H
#define ACTIO_LIGHT_POINT_H

#include "../../lib/types.h"

namespace game {
    namespace desc {
        struct LightPointDesc {
            LightPointDesc() = default;

            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec3 position;
            vec3 direction;
            float constant;
            float linear;
            float quadratic;

            void setAmbient(vec3 value) { ambient = value; };
            void setDiffuse(vec3 value) { diffuse = value; };
            void setSpecular(vec3 value) { specular = value; };
            void setPosition(vec3 value) { position = value; };
            void setDirection(vec3 value) { direction = value; };
            void setConstant(float value) { constant = value; };
            void setLinear(float value) { linear = value; };
            void setQuadratic(float value) { quadratic = value; };

            vec3 getAmbient() const { return ambient; };
            vec3 getDiffuse() const { return diffuse; };
            vec3 getSpecular() const { return specular; };
            vec3 getPosition() const { return position; };
            vec3 getDirection() const { return direction; };
            float getConstant() const { return constant; };
            float getLinear() const { return linear; };
            float getQuadratic() const { return quadratic; };
        };
    }
}

#endif //ACTIO_LIGHT_POINT_H
