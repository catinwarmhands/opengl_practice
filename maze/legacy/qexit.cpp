#pragma once

#include <cstdlib>
#include <vector>
#include <functional>

#include <cstdlib>

// #if !defined(_GLIBCXX_HAVE_AT_QUICK_EXIT) 
// #error error1
// #endif

// #if !defined(_GLIBCXX_HAVE_QUICK_EXIT)
// #error error2
// #endif


namespace _at_qexit {
	std::vector<std::function<void()>> actions;
}

#define defer_qexit(code) _at_qexit::actions.push_back([&](){code;}) 

#define QEXIT {                                        \
	auto& v = _at_qexit::actions;                      \
	for(auto it = v.rbegin();  it != v.rend(); ++it) { \
		(*it)();                                       \
	}                                                  \
	/*FIXME gcc 6.2 does not support quick_exit...*/   \
	exit(0);                                           \
}

#define QEXIT_PAUSE { \
	system("pause");  \
	QEXIT;            \
}
