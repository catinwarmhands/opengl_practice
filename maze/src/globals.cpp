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
ivec2 mazeSize;
int* mazeMatrix = nullptr;
Mesh mazeMesh;
Model mazeModel;

Mesh groundMesh;
Model groundModel;

Model cubeModel;
vector<vec3> cubesPositions;

Model coinModel;
vector<vec3> coinsPositions;

Player player;
Model playerModel;
Camera2D camera;

vector<GLuint> vertexShaders;
vector<GLuint> fragmentShaders;
vector<GLuint> shaderPrograms;
vector<GLuint> textures;
GLuint currentShader;

int score = 0;
double bestTime = -1;
double winTime = -1;
bool isWin = false;

bool noclipMode = false;
bool firstPersonMode = true;
bool wireframeMode = false;
bool mouseLock = true;
bool fullScreen = false;

// mat4 model;
mat4 view;
mat4 projection;


float cameraSpeed = 5.0f;
float mouseSensitivity = 0.1f;

InputState input;


GLuint cobblestone;
GLuint grass;
GLuint lev;
GLuint cat;
GLuint coin;


FONScontext* fs = NULL;
int fontNormal = FONS_INVALID;
unsigned int white = glfonsRGBA(255,255,255,255);
unsigned int red   = glfonsRGBA(255,0,0,255);

string scoreMessage;
string timeMessage;
string bestTimeMessage;
