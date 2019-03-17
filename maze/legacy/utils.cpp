#pragma once

#include "settings/settings.h"

#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <cstdlib>
#include <cstdint>
#include <ctime>
#include <cstring>

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <chrono>
#include <thread>
#include <mutex>
#include <windows.h>

#include "../../libs/defer.hpp"

#include "../../libs/timer.hpp"

#include "../../libs/glew/include/GL/glew.h"

#include "../../libs/glfw64/include/GLFW/glfw3.h"

#include "qexit.cpp"
#include "logger.cpp"

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using byte = u8;

using float32 = float;
using float64 = double;

using f32 = float32;
using f64 = float64;

using String = std::string;

template<class T>
using DynArray = std::vector<T>;

template<class T>
using Deque = std::deque<T>;

#define varprint(x) std::cout << #x << " = " << (x) << '\n'

#define KB *1024
#define MB KB *1024
#define GB MB *1024

template<class T>
T random(T min, T max) {
	return min + (T)(rand()) /(T(RAND_MAX/(max-min+1)));
}

bool ends_with(const String& filePath, const String& extension) {
	u64 len1 = filePath.length();
	u64 len2 = extension.length();

	if (len1 < len2) return false;

	u64 k = 0;
	for (auto i = len1-len2; i < len1; ++i)
		if (filePath[i] != extension[k++])
			return false;

	return true;
}

String read_file(String filePath) {
	FILE* file = fopen(filePath.c_str(), "rt");

	if (file == nullptr) {
		logger.fatal("[read_file] Cant open file '{}'", filePath);
	}

	fseek(file, 0, SEEK_END);
	unsigned long length = ftell(file);
	char* data = new char[length + 1];
	memset(data, 0, length + 1);
	fseek(file, 0, SEEK_SET);

	fread(data, 1, length, file);
	fclose(file);

	String result(data);
	delete[] data;
	return result;
}

//check if file exist
#ifdef _WIN32
   #include <io.h> 
#else
   #include <unistd.h>
#endif

bool file_exists( const std::string &Filename ) {
	return access(Filename.c_str(), 0) == 0;
	// return
	// #ifdef _WIN32
	//     _access_s
	// #else
	//     access
	// #endif
	//     (Filename.c_str(), 0) == 0;
}
