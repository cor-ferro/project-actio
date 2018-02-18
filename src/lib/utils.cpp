#include "utils.h"

namespace utils {
	std::string formatMemorySize(std::size_t size) {
		std::stringstream ss;

		if (size >= 1073741824) {
			ss << (size / 1024 / 1024) << " " << "Gb";
		} else if (size >= 1048576) {
			ss << (size / 1024 / 1024) << " " << "Mb";
		} else if (size > 1000) {
			ss << (size / 1024) << " " << "Kb";
		} else {
			ss << (size) << " " << "byte";
		}

		std::string finalStr = ss.str();

		return finalStr;
	}
}
