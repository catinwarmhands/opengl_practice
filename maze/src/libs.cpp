#pragma once

// стандартные библиотеки
#include <iostream> //cout, endl
#include <iomanip> //fixed
#include <sstream> //istringstream
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

// stb_image
#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wconstant-conversion"
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "../libs/stb/stb_image.h"
#ifdef __clang__
	#pragma clang diagnostic pop
#endif

// tinyobjloader
#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif
#include "../libs/tinyobjloader/tiny_obj_loader.h"
#include "../libs/tinyobjloader/tiny_obj_loader.cc"
#ifdef __clang__
	#pragma clang diagnostic pop
#endif

// fontstash
#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#define FONTSTASH_IMPLEMENTATION
#include "../libs/fontstash/src/fontstash.h"
#define GLFONTSTASH_IMPLEMENTATION	
#include "../libs/fontstash/src/glfontstash.h"

#ifdef __clang__
	#pragma clang diagnostic pop
#endif
