#ifndef TYPES_H_
#define TYPES_H_

#include <stdalign.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using vec2 = glm::vec2 ;
using vec3 = glm::vec3 ;
using vec4 = glm::vec4 ;
using ivec4 = glm::ivec4 ;
using mat2 = glm::mat2 ;
using mat3 = glm::mat3 ;
using mat4 = glm::mat4 ;
using quat = glm::quat ;

const float fE = glm::e<float>();
const double dE = glm::e<double>();

struct alignas(16) vec3_16 {};

namespace color {
    using Rg = glm::vec2 ;
    using Rgb = glm::vec3 ;
    using Hsv = glm::vec3 ;

    using Val = float;
}

#endif /* TYPES_H_ */
