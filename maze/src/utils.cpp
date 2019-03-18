#pragma once

#include "libs.cpp"

// знак числа {-1, 0, 1}
template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// вывести значение переменной иеё название на экран (для дебага)
#define varprint(x) std::cout << #x << " = " << (x) << std::endl

// заканчивается строка другой строкой
bool ends_with(const string& s1, const string& s2) {
	return s1.rfind(s2) == (s1.length()-s2.length());
}

// прочитать весь файл и вернуть его в виде строки
string read_file(const string& filePath) {
	// игнорируем предупреждение "fopen is deprecated"
	#ifdef __clang__
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wdeprecated-declarations"
	#endif

	FILE* file = fopen(filePath.c_str(), "rt");

	if (file == nullptr) {
		cout << "[read_file] Cant open file '" << filePath << "'";
		exit(1);
		return "";
	}

	fseek(file, 0, SEEK_END);
	unsigned long length = ftell(file);
	char* data = new char[length + 1];
	memset(data, 0, length + 1);
	fseek(file, 0, SEEK_SET);
	fread(data, 1, length, file);
	fclose(file);
	string result(data);
	delete[] data;
	return result;
}

// узнать, в какой папке лежит проект
string get_root_path() 
{
	#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		char buffer[MAX_PATH];
	    GetModuleFileName(NULL, buffer, MAX_PATH);
	    string s(buffer);
	    int count = 0;
	    for (int i = s.length()-1; i >= 0; --i)
	    {
	    	if (s[i] == '\\' || s[i] == '/')
	    		++count;
	    	if (count == 2) {
	    		return s.substr(0, i+1);

	    	}
	    }
        return "";
	#else
		#error [root_path] only works on windows for now...
	#endif
}