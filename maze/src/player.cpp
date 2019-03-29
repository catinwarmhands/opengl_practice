#pragma once

#include "libs.cpp"

struct Player {
	vec3 position;
	float orientation = -90;
	float size = 0.5;
	float speed = 5.0f;
};