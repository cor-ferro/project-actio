/*
 * spot_light.h
 *
 *  Created on: Aug 30, 2017
 *      Author: demitriy
 */

#ifndef SPOT_LIGHT_H_
#define SPOT_LIGHT_H_

#include "light.h"
#include "../lib/types.h"

namespace Light {
    struct Spot {
        Spot();

        Spot(vec3 ambient, vec3 diffuse, vec3 specular);

        void setAmbient(const vec3& ambient);

        void setAmbient(float r, float g, float b);

        void setDiffuse(const vec3& diffuse);

        void setDiffuse(float r, float g, float b);

        void setSpecular(const vec3& specular);

        void setSpecular(float r, float g, float b);

        const vec3& getAmbient();

        const vec3& getDiffuse();

        const vec3& getSpecular();

        const vec3& getPosition();

        const vec3& getDirection();

        void setPosition(const vec3& position);

        void setDirection(const vec3& direction);

        void setAttenuation(float constant, float linear, float quadratic);

        void setCutoff(float cutOff, float outerCutOff);

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


#endif /* SPOT_LIGHT_H_ */
