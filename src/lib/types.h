#ifndef TYPES_H_
#define TYPES_H_

#include <stdalign.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::ivec4 ivec4;
typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::quat quat;

const float fE = glm::e<float>();
const double dE = glm::e<double>();

struct alignas(16) vec3_16 {};

namespace color {
    typedef glm::vec2 Rg;
    typedef glm::vec3 Rgb;
    typedef glm::vec3 Hsv;

    typedef float Val;
}

#endif /* TYPES_H_ */
