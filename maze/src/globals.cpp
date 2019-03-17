#include "libs.cpp"

#include "player.cpp"
#include "camera.cpp"

// путь до экзешника
string rootPath;

// окно
GLFWwindow* window;
const int WIDTH  = 640;
const int HEIGHT = 480;

// номер текущего кадра
int frameNumber = 0;

// время
double previousTime = 0;
double currentTime  = 0;
double dt           = 0;

// размеры лабиринта и его матрица
const int MAZE_N = 51;
const int MAZE_M = 51;
int* maze = nullptr;

Player player;
Camera2D camera;
vec2 cursorPosition;

vector<GLuint> vertexShaders;
vector<GLuint> fragmentShaders;
vector<GLuint> shaderPrograms;

int mode  = 1;
int modeN = 3;