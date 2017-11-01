#ifndef APP_H_
#define APP_H_

#include <string>
#include <boost/filesystem/path.hpp>
#include "../lib/console.h"

struct App {
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

	App& operator ->() {
		return App::instance();
	}
private:
	App(App const &);
	App& operator= (App const &);

	std::string name_;
	std::string path_;

	int argc_;
	char **argv_;
};



#endif /* APP_H_ */
