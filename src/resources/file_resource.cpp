#include "file_resource.h"

namespace Resource {
	File::File() {};

	File::File(const File& other)
	{
		path_ = other.path_;
	}

	File::File(std::string path) 
		: path_(App::instance().resourcePath(path))
	{
		console::info("resource::file ", path_.string());
	}
	
	File::~File() {}

	std::string File::getPath()
	{
		return path_.string();
	}

}
