//
// Created by demitriy on 3/29/18.
//

#ifndef ACTIO_GAME_LIGHT_POINT_H
#define ACTIO_GAME_LIGHT_POINT_H


#include "../../lights/point_light.h"

namespace game {
    namespace components {
        struct LightPoint : Light::Point {
            LightPoint(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular, float c, float l, float q)
                    : Light::Point(position, ambient, diffuse, specular) {
                setAttenuation(c, l, q);
            };

            LightPoint(vec3 position, vec3 ambient, vec3 diffuse, vec3 specular)
                    : Light::Point(position, ambient, diffuse, specular) {}

            LightPoint()
                    : Light::Point() {}
        };
    }
}

#endif //ACTIO_LIGHT_POINT_H
