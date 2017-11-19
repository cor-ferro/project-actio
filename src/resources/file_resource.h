/*
 * file_resource.h
 *
 *  Created on: Oct 14, 2017
 *      Author: demitriy
 */

#ifndef FILE_RESOURCE_H_
#define FILE_RESOURCE_H_

#include <string>
#include <boost/filesystem/path.hpp>
#include "../app/app.h"

namespace Resource {

	struct File {
		File();
		File(std::string path);
		File(const File& other);
		~File();

		std::string getPath();

	private:
		boost::filesystem::path path_;
	};

}

#endif /* FILE_RESOURCE_H_ */
