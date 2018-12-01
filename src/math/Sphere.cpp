#include "Sphere.h"

namespace Math {
    Sphere::Sphere() {}

    Sphere::Sphere(const Sphere& other)
    {
        center = other.center;
        size = other.size;
    }
}