#ifndef MATH_BOX3_H
#define MATH_BOX3_H

#include <glm/glm.hpp>
#include "../lib/types.h"

namespace Math {
    struct Box3 {
        Box3();
        Box3(const Box3& other);
        
        bool isEmpty();

        vec3 center;
        vec3 radius;
    };
}

#endif