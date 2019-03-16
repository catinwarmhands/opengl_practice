#pragma once

template<class T>
T random(T min, T max) {
	return min + (T)(rand()) /(T(RAND_MAX/(max-min+1)));
}

template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}