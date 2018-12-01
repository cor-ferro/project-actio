#include "Box3.h"

namespace Math {
    Box3::Box3() {}

    Box3::Box3(const Box3& other)
    {
        // size = other.size;
    }

    bool Box3::isEmpty()
    {
//        return size == 0.0f;
        return false;
    }
}