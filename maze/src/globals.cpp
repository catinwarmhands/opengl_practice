#pragma once

#include "libs.cpp"

#include "player.cpp"
#include "camera.cpp"
#include "input.cpp"
#include "model.cpp"

// путь до корневого каталога (с ресурсами)
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

// лабиринт
ivec2 mazeSize;
int* mazeMatrix = nullptr;
Mesh mazeMesh;
Model mazeModel;

// земля
Mesh groundMesh;
Model groundModel;

// кубики
Model cubeModel;
vector<vec3> cubesPositions;

// монетки
Model coinModel;
vector<vec3> coinsPositions;

// игрок
Player player;
Model playerModel;

// камера
Camera2D camera;

// шейдеры
vector<GLuint> vertexShaders;
vector<GLuint> fragmentShaders;
vector<GLuint> shaderPrograms;
GLuint currentShader;

// текстурки
GLuint cobblestone;
GLuint grass;
GLuint lev;
GLuint cat;
GLuint coin;

// количество собранных монет и лучшее время
int score = 0;
double bestTime = -1;
double winTime = -1;
bool isWin = false;

// режимы
bool noclipMode = false;
bool firstPersonMode = true;
bool wireframeMode = false;
bool mouseLock = true;
bool fullScreen = false;

// минимальное значение зума для камер
float minFov = 30;
float minScale = 2;

// матрицы вида и проекции
mat4 view;
mat4 projection;

// чувствительность мыши
float mouseSensitivity = 0.1f;

// тут хранится текущее состояние инпута
InputState input;


// рисовалка текста
FONScontext* fs = NULL;
int fontNormal = FONS_INVALID;
// цвета для неё
unsigned int white = glfonsRGBA(255,255,255,255);
unsigned int red   = glfonsRGBA(255,0,0,255);

// строки с четом, временем, рекордом
string scoreMessage;
string timeMessage;
string bestTimeMessage;
string noclipMessage = "NOCLIP mode";
string winMessage = "Победа!";
