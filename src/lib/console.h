#ifndef CONSOLE_H_
#define CONSOLE_H_

#define CONSOLE_ESC "\033"
#define CONSOLE_RESET "[0m"
#define COLOR_CYAN "[36m"
#define COLOR_YELLOW "[33m"
#define COLOR_RED "[31m"

#include <iostream>

namespace console {
	template <typename ...Arg>
	static void log(Arg... arg)
	{
		using expander = int[];
		(void) expander{ (std::cout << " " << std::forward<Arg>(arg), void(), 0)... };
		std::cout << std::endl;
	}

	template <typename ...Arg>
	static void info(Arg... arg)
	{
		std::cout << CONSOLE_ESC COLOR_CYAN;
		std::cout << "[info]";
		std::cout << CONSOLE_ESC CONSOLE_RESET;
		log(arg...);
	}

	template <typename ...Arg>
	static void warn(Arg... arg)
	{
		std::cout << CONSOLE_ESC COLOR_YELLOW;
		std::cout << "[warn]";
		std::cout << CONSOLE_ESC CONSOLE_RESET;
		log(arg...);
	}

	template <typename ...Arg>
	static void err(Arg... arg)
	{
		std::cout << CONSOLE_ESC COLOR_RED;
		std::cout << "[err] ";
		std::cout << CONSOLE_ESC CONSOLE_RESET;
		log(arg...);
	}
};

#endif
