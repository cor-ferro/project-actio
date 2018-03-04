#ifndef LIB_COMPARATORS_H
#define LIB_COMPARATORS_H

#include <cstring>
#include "console.h"

namespace comparators {
    struct chars {
        bool operator()(const char *a, const char *b) {
            console::infop("compare: %s, %s", a, b);
            return std::strcmp(a, b) == 0;
        }
    };
}

#endif