/*
 * buffer_resource.h
 *
 *  Created on: Oct 14, 2017
 *      Author: demitriy
 */

#ifndef BUFFER_RESOURCE_H_
#define BUFFER_RESOURCE_H_

#include <string>

namespace Resource {

	struct Buffer {
		Buffer();
		Buffer(std::string path);
		~Buffer();
	};

}


#endif /* BUFFER_RESOURCE_H_ */
