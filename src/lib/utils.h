#ifndef LIB_UTILS_H_
#define LIB_UTILS_H_

#include <string>
#include <sstream>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

namespace utils {
	std::string formatMemorySize(std::size_t size);
}


#endif /* LIB_UTILS_H_ */
