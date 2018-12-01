#ifndef CONSOLE_H_
#define CONSOLE_H_

#define CONSOLE_ESC "\033"
#define CONSOLE_RESET "[0m"
#define COLOR_CYAN "[36m"
#define COLOR_YELLOW "[33m"
#define COLOR_RED "[31m"

#include <iostream>
#include <boost/format.hpp>
#include <glm/gtx/string_cast.hpp>

namespace console {
	template <typename ...Arg>
	static void log(const char * format, Arg... args) {
		boost::format f(format);
		int unroll[] {0, (f % std::forward<Arg>(args), 0)...};
		static_cast<void>(unroll);

		std::cout << boost::str(f);
		std::cout << std::endl;
	}

	template <typename ...Arg>
	static void info(const char * format, Arg... arg) {
		std::cout << CONSOLE_ESC COLOR_CYAN;
		std::cout << "[info] ";
		std::cout << CONSOLE_ESC CONSOLE_RESET;
		log(format, arg...);
	}

	template <typename ...Arg>
	static void warn(const char * format, Arg... arg) {
		std::cout << CONSOLE_ESC COLOR_YELLOW;
		std::cout << "[warn] ";
		std::cout << CONSOLE_ESC CONSOLE_RESET;
		log(format, arg...);
	}

	template <typename ...Arg>
	static void err(const char * format, Arg... arg) {
		std::cout << CONSOLE_ESC COLOR_RED;
		std::cout << "[err] ";
		std::cout << CONSOLE_ESC CONSOLE_RESET;
		log(format, arg...);
	}

	template <typename T>
	static void geom(const char * format, T t) {
		console::log(format, glm::to_string(t));
	}
};

#endif
