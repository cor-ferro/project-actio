#ifndef MATH_SPHERE_H
#define MATH_SPHERE_H

#include <glm/glm.hpp>
#include "../lib/types.h"
#include "Box3.h"

namespace Math {
    struct Sphere {
        Sphere();
        Sphere(const Sphere& other);

        float size;
    private:
        vec3 center;
    };
}

#endif