#ifndef LIB_UTILS_H_
#define LIB_UTILS_H_

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "types.h"
#include <fstream>
#include <streambuf>
#include <string>
#include <cerrno>
#include "path.h"
#include "data.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

namespace utils {
    std::string formatMemorySize(std::size_t size);

    const std::vector<std::string> parseVec(const std::string &strVec);

    template<class T>
    const T stovec(const std::string &strVec);

    std::vector<std::string> split(std::string str, char separator);

    Data getFileContents(const Path& path);

    void getFileContents(const Path &path, Data& data);

    float scaleNumber(float x, float y, float value);

    template <typename T>
    inline T toZero(T value, T reduceValue) {
        return value > static_cast<T>(0)
               ? value - reduceValue
               : value + reduceValue;
    }
}


#endif /* LIB_UTILS_H_ */
