#include "libs.cpp"

#include "player.cpp"


// размер окна
const int WIDTH  = 640;
const int HEIGHT = 480;

// номер текущего кадра
int frame_number = 0;

// последний замер времени
double last_time = 0;



Player player;

GLFWwindow* window;

// размеры лабиринта и указатель на его матрицу
const int MAZE_N = 51;
const int MAZE_M = 51;
int* maze = nullptr;
