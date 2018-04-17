//
// Created by demitriy on 4/15/18.
//

#ifndef ACTIO_MATH_H
#define ACTIO_MATH_H

#include <glm/glm/common.hpp>

namespace math {
    float scale(float x, float y, float value) {
        float v = glm::clamp(value, x, y);
        float dist = glm::abs(y) - glm::abs(x);

        return (v - x) / dist;
    }

    template <typename T>
    inline T toZero(T value, T reduceValue) {
        return value > static_cast<T>(0)
                ? value - reduceValue
                : value + reduceValue;
    }
}

#endif //ACTIO_MATH_H
