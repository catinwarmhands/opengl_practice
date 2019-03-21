#include "libs.cpp"

#include "player.cpp"
#include "camera.cpp"

// путь до экзешника
string rootPath;

// окно
GLFWwindow* window;
ivec2 windowSize = {640, 480};
ivec2 frameBufferSize;

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

vector<GLfloat> vertices;
vector<GLuint> indices;
GLuint VBO, VAO, EBO;

vector<GLuint> vertexShaders;
vector<GLuint> fragmentShaders;
vector<GLuint> shaderPrograms;

int mode  = 1;
int modeN = 3;

vector<GLuint> textures;

bool wireframeMode = false;

mat4 model;
mat4 view;
mat4 projection;

vector<vec3> cubePositions;
