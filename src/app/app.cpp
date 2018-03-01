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

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glfwSetErrorCallback(w1ErrorCallback);

	int countMonitors = 0;
	GLFWmonitor ** monitors = glfwGetMonitors(&countMonitors);

	for (int i = 0; i < countMonitors; i++) {
		addMonitor(monitors[i]);
	}

	glfwSetMonitorCallback([](GLFWmonitor* monitor, int event) {
		switch (event) {
			case GLFW_CONNECTED:
				console::info("monitor connected");
				// addMonitor(monitor);
				break;

			case GLFW_DISCONNECTED:
				console::info("monitor disconnected");
				// this->removeMonitor(monitor);
				break;
		}
	});
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

void App::addMonitor(GLFWmonitor * monitor)
{
	monitors.insert({ (void*)monitor,  Monitor(monitor) });
}

void App::removeMonitor(GLFWmonitor * monitor)
{
	auto it = monitors.find((void*)monitor);

	if (it == monitors.end()) {
		console::warn("try remove non exists monitor");
		return;
	}

	monitors.erase(it);
}

const Monitor * const App::getPrimaryMonitor()
{
	GLFWmonitor * ptr = glfwGetPrimaryMonitor();

	auto it = monitors.find((void*)ptr);

	if (it == monitors.end()) {
		return nullptr;
	}

	return &it->second;
}