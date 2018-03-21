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
#include "../lib/console.h"

namespace Resource {

	struct File {
		File();
		File(std::string path);
		File(const File& other);
		~File();

		std::string getPath() const;

	private:
		boost::filesystem::path path_;
	};

}

#endif /* FILE_RESOURCE_H_ */
