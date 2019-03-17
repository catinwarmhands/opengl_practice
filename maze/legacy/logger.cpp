#pragma once

#include "../../libs/fmt/format.h"
#include "../../libs/fmt/format.cc"
#include "../../libs/fmt/ostream.h"
// #include "../../libs/fmt/time.h"

#include "../../libs/termcolor.hpp"

struct Logger {
	template<class ... Types>
	void log(Types ... args) {
		// auto t = std::time(nullptr);
		// fmt::print("[{:%H:%M:%S}] ", *std::localtime(&t));
		fmt::print(args...);
		fmt::print("\n");
	}

	template<class ... Types>
	void fatal(Types ... args) {
		std::cout << termcolor::red << termcolor::on_white;
		this->log(args...);
		std::cout << termcolor::reset;
		fmt::print("Fatal error occurred, calling QEXIT_PAUSE\n");
		QEXIT_PAUSE;
	}
	
	template<class ... Types>
	void error(Types ... args) {
		std::cout << termcolor::white << termcolor::on_red;
		this->log(args...);
		std::cout << termcolor::reset;
	}

	template<class ... Types>
	void warning(Types ... args) {
		std::cout << termcolor::yellow;
		this->log(args...);
		std::cout << termcolor::reset;
	}

	template<class ... Types>
	void info(Types ... args) {
		std::cout << termcolor::cyan;
		this->log(args...);
		std::cout << termcolor::reset;
	}

	template<class ... Types>
	void good(Types ... args) {
		std::cout << termcolor::green;
		this->log(args...);
		std::cout << termcolor::reset;
	}
};

Logger logger;