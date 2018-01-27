/*
 * colors.h
 *
 *  Created on: Jan 27, 2018
 *      Author: demitriy
 */

#ifndef SRC_LIB_COLORS_H_
#define SRC_LIB_COLORS_H_

#include "types.h"

namespace colors {
	inline vec3 green() {
		return vec3(0.0f, 1.0f, 0.0f);
	}

	inline vec3 red() {
		return vec3(1.0f, 0.0f, 0.0f);
	}

	inline vec3 blue() {
		return vec3(0.0f, 0.0f, 1.0f);
	}

	inline vec3 white() {
		return vec3(1.0f);
	}

	inline vec3 black() {
		return vec3(0.0f);
	}
}

#endif /* SRC_LIB_COLORS_H_ */
