#include "app.h"

App::App()
{
	console::warn("create app");
	name_ = "";
	argc_ = 0;
	argv_ = 0;
}

void App::init(int argc, char **argv)
{
	argc_ = argc;
	argv_ = argv;

	path_ = ".";
	console::info("init path", path_);
}

std::string App::getName()
{
	return name_;
}

std::string App::getPath()
{
	return path_;
}

std::string App::resourcePath()
{
	return path_ + "/resources";
}

std::string App::resourcePath(std::string fromPath)
{
	return path_ + "/resources/" + fromPath;
}

std::string App::shadersPath()
{
	return path_ + "/shaders/";
}

std::string App::shadersPath(std::string fromPath)
{
	return path_ + "/shaders/" + fromPath;
}

void App::setName(std::string name)
{
	name_ = name;
}
