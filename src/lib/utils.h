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

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

namespace utils {
    std::string formatMemorySize(std::size_t size);

    const std::vector<std::string> parseVec(const std::string &strVec);

    template<class T>
    const T stovec(const std::string &strVec);

    std::vector<std::string> split(std::string str, char separator);

    std::string getFileContents(Path path);

    void getFileContents(Path path, std::shared_ptr<std::string> &data, size_t *size);
}


#endif /* LIB_UTILS_H_ */
