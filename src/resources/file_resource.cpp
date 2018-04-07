#include "file_resource.h"
#include "../lib/path.h"

#ifndef RESOURCE_DIR
#error "undefined RESOURCE_DIR"
#endif

namespace Resource {
	File::File() {};

	File::File(const File& other)
	{
		path_ = other.path_;
	}

	File::File(std::string path) 
		: path_(createPath(path))
	{
		console::info("resource::file %s", path_.string());
	}
	
	File::~File() {}

	std::string File::getPath() const
	{
		return path_.string();
	}

}
