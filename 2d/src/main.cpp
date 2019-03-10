#include <iostream> //cout, endl
#include <cmath> //sin, cos
using namespace std;

// библиотеки OpenGL
#include "../libs/glew/include/GL/glew.h"
#include "../libs/glfw64/include/GLFW/glfw3.h"

// размер окна
const int WIDTH  = 640;
const int HEIGHT = 480;

// наш код, который будет выполняться каждый кадр
void loop() {

	// цвет очистки экрана - белый
	glClearColor(1, 1, 1, 0);

	// очищаем экран
	glClear(GL_COLOR_BUFFER_BIT);

	// рисуем:

	// серый квадрат
	glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_QUADS);
		glVertex2f(-0.8, -0.6);
		glVertex2f( 0.8, -0.6);
		glVertex2f( 0.8,  0.6);
		glVertex2f(-0.8,  0.6);
	glEnd();

	// красный круг
	{
		float x = 0;
		float y = 0.6;
		float radius = 0.4;
		float twicePi = 6.28318530718;
		int n = 50;
		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x, y); // центр
			for(int i = 0; i <= n; i++) { 
				glVertex2f(
		            x + (radius * cos(i * twicePi / n)), 
				    y + (radius * sin(i * twicePi / n))
				);
			}
		glEnd();
	}
	
	// зеленый треугольник
	glColor3f(0, 1, 0);
    glBegin(GL_POLYGON);
		glVertex2f(-0.7, -0.85);
		glVertex2f( 0.7, -0.85);
		glVertex2f( 0.0,  0.4);
	glEnd();
}

// функция, обрабатывающая нажатие на кнопки
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) //если нажали на ESC
        glfwSetWindowShouldClose(window, GLFW_TRUE); //то окно надо закрыть
}

int main() {
	// запускаем glfw
	if (!glfwInit()) {
		// если не запускается, то выключаемся
		cout << "[GLFW] failed to init" << endl;
		return 1;
	}

	// создаем окно
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);

	// если не создалось
	if (!window) {
		// то отключаемся
		cout << "[GLFW] failed to open window" << endl;
		glfwTerminate();
		return 1;
	}

	// будем рисовать в это окно
	glfwMakeContextCurrent(window);

	// назначаем нашу функцию для обработки нажатий на клавиатуру
	glfwSetKeyCallback(window, key_callback);

	// настраеваем камеру (что бы фигуры не растягивало)
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	float aspect = (float)WIDTH / (float)HEIGHT;
	glOrtho(-aspect, aspect, -1, 1, -1, 1);

	// пока окно не должно закрыться
	while (!glfwWindowShouldClose(window)) {

		// выполняем наш код
		loop();

		// выводим на экран (это вместо glFlush)
		glfwSwapBuffers(window);

		// опрашиваем окно
		glfwPollEvents();
	}

	// закрываем glfw
	glfwTerminate();
}