#pragma once

#include "libs.cpp"

// знак числа {-1, 0, 1}
template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// вывести значение переменной иеё название на экран (для дебага)
#define varprint(x) cout << #x << " = " << (x) << endl

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


vector<string> split_string(const string& str, const string& delimiters) {
	size_t start = 0;
	size_t end = str.find_first_of(delimiters);
	vector<string> result;
	while (end <= string::npos) {
		string token = str.substr(start, end - start);
		if (!token.empty())
			result.push_back(token);
		if (end == string::npos)
			break;
		start = end + 1;
		end = str.find_first_of(delimiters, start);
	}
	return result;
}

vector<string> split_string(const string& str, const char delimiter) {
	return split_string(str, string(1, delimiter));
}

vector<string> tokenize_string(const string& str) {
	return split_string(str, " \t\n");
}

vector<string> GetLines(const string& str) {
	return split_string(str, "\n");
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

template<class T>
string to_string(const vector<T>& vec) {
	int n = vec.size();
	std::ostringstream oss;
	oss << "{";
	for (int i = 0; i < n; ++i) {
		oss << vec[i];
		if (i != n-1)
			oss << ", ";
	}
	oss << "}";
	return oss.str();
}

template<class T>
string to_string(const T& x) {
	std::ostringstream oss;
	oss << x;
	return oss.str();
}