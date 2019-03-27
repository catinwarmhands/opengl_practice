#pragma once

#include "libs.cpp"

#include "player.cpp"
#include "camera.cpp"
#include "input.cpp"
#include "model.cpp"

// путь до экзешника
string rootPath;

// окно
GLFWwindow* window;
ivec2 defaultWindowSize = {640, 480};
ivec2 windowSize = defaultWindowSize;
ivec2 frameBufferSize;

// номер текущего кадра
int frameNumber = 0;

// время
double previousTime = 0;
double currentTime  = 0;
double dt           = 0;

// размеры лабиринта и его матрица
const int MAZE_N = 15;
const int MAZE_M = 15;
int* mazeMatrix = nullptr;
Mesh mazeMesh;
Model mazeModel;

Player player;
Model playerModel;
Camera2D camera;


vector<GLuint> vertexShaders;
vector<GLuint> fragmentShaders;
vector<GLuint> shaderPrograms;

bool firstPersonMode = true;
vector<GLuint> textures;

bool wireframeMode = false;
bool mouseLock = true;
bool fullScreen = false;

mat4 model;
mat4 view;
mat4 projection;

vector<vec3> cubePositions;

float cameraSpeed = 5.0f;
float mouseSensitivity = 0.1f;

InputState input;

Model cube;

GLuint cobblestone;
GLuint lev;
GLuint cat;