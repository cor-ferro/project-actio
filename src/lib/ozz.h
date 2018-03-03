#ifndef LIB_OZZ_H_
#define LIB_OZZ_H_

#include "./types.h"
#include "./console.h"
#include "ozz/base/maths/simd_math.h"

namespace libOzz {
    mat4 toNativeType(ozz::math::Float4x4& ozzMat);
}

#endif /* LIB_OZZ_H_ */