#pragma once

#include "libs.cpp"

struct InputState {
	bool keys[1024];
	vec2 cursorPosition;
	vec2 cursorOffset;
	// тут ещё можно геймпад добавить
	// https://github.com/BastiaanOlij/glfw-tutorial/blob/master/archive/22.%20Heightfield%20%231/include/joysticks.h
};