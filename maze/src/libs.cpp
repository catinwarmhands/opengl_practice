#pragma once

// стандартные библиотеки
#include <iostream> //cout, endl
#include <cmath> //sin, cos
#include <string>
#include <stack>
#include <vector>
#include <windows.h> //Sleep
#include <time.h> // для srand
using namespace std;

// библиотеки OpenGL
#define GLEW_STATIC
#include "../libs/glew/include/GL/glew.h"
#include "../libs/glfw64/include/GLFW/glfw3.h"

// glm
#include "../libs/glm/glm.hpp"
#include "../libs/glm/gtx/io.hpp"
#include "../libs/glm/gtc/type_ptr.hpp"
#include "../libs/glm/gtc/matrix_transform.hpp"
#include "../libs/glm/gtc/random.hpp"
using namespace glm;
