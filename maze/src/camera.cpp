#pragma once

#include "libs.cpp"

struct Camera2D {
	vec3 position = vec3(0.0f, 0.0f,  3.0f);
	vec3 front    = vec3(0.0f, 0.0f, -1.0f);
	vec3 up       = vec3(0.0f, 1.0f,  0.0f);
	float yaw     = 0;
	float pitch   = 0;

};