#ifndef APP_H_
#define APP_H_

#include <string>
#include <unordered_map>
#include <functional>
#include <boost/filesystem/path.hpp>
#include "../lib/console.h"

#include "monitor.h"

static void w1ErrorCallback(int error, const char* description)
{
    console::err("GLFW error: ", description);
}

struct App {
	typedef std::unordered_map<void*, Monitor> Monitors;

	static App& instance() {
		static App app;

		return app;
	}

	App();

	void init(int argc, char **argv);

	std::string getName();
	std::string getPath();
	std::string resourcePath();
	std::string resourcePath(std::string fromPath);
	std::string shadersPath();
	std::string shadersPath(std::string fromPath);
	void setName(std::string name);

	const Monitor * const getPrimaryMonitor();
	const Monitors& getMonitors();

	App& operator ->() {
		return App::instance();
	}
private:
	App(App const &);
	App& operator= (App const &);

	std::string name_;
	std::string path_;

	Monitors monitors;
	void addMonitor(GLFWmonitor * monitor);
	void removeMonitor(GLFWmonitor * monitor);

	int argc_;
	char **argv_;
};



#endif /* APP_H_ */
