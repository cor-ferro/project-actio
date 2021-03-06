/*
 * direction_light.h
 *
 *  Created on: Aug 29, 2017
 *      Author: demitriy
 */

#ifndef DIRECTION_LIGHT_H_
#define DIRECTION_LIGHT_H_

#include "light.h"
#include "../lib/types.h"

namespace Light {
    struct Directional {
        Directional();

        Directional(vec3 ambient, vec3 diffuse, vec3 specular);

        Directional(vec3 ambient, vec3 diffuse, vec3 specular, vec3 direction);

        void setAmbient(vec3 ambient);

        void setAmbient(float r, float g, float b);

        void setDiffuse(vec3 diffuse);

        void setDiffuse(float r, float g, float b);

        void setSpecular(vec3 specular);

        void setSpecular(float r, float g, float b);

        void setDirection(vec3 specular);

        void setDirection(float r, float g, float b);

        vec3 getAmbient();

        vec3 getDiffuse();

        vec3 getSpecular();

        vec3 getDirection();

        // keep same order
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        vec3 direction;
    };
} // Light

#endif /* DIRECTION_LIGHT_H_ */
