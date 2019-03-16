#include "libs.cpp"
#include "globals.cpp"
#include "utils.cpp"

// сгенерировать матрицу лабиринта размера n строк на m столбцов
// n и m доблжны быть нечетными
// алгоритм: https://habr.com/ru/post/262345/
int* generate_maze_matrix(int n, int m) {
	enum CellType {
		UNVISITED = -1,
		EMPTY     =  0,
		WALL      =  1
	};

	int* maze = new int[n*m];

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			int pos = i*m+j;
			if (i % 2 == 0 || j % 2 == 0)
				maze[pos] = WALL;
			else
				maze[pos] = UNVISITED;
		}
	}

	ivec2 cur = ivec2(1,1);
	stack<ivec2> history;
	while (true) {
		int unvisided_index = -1;
		for (int i = 0; i < n*m; ++i)
			if (maze[i] == UNVISITED)
				unvisided_index = i;

		if (unvisided_index == -1)
			break;

		vector<ivec2> neighbours;
		for (int i = 0; i < 4; ++i) {
			ivec2 v = cur;
			switch (i) {
				case 0: v += ivec2(-2, 0); break;
				case 1: v += ivec2( 2, 0); break;
				case 2: v += ivec2( 0,-2); break;
				case 3: v += ivec2( 0, 2); break;
			}
			if (0 <= v.x && v.x < m && 0 <= v.y && v.y < n && maze[v.y*m + v.x])
				neighbours.push_back(v);
		}

		if (neighbours.size() != 0) {
			history.push(cur);
			ivec2 target = neighbours[random(0, (int)neighbours.size()-1)];
			ivec2 wall = cur + ivec2(-sgn(cur.x-target.x), -sgn(cur.y-target.y));
			maze[cur.y*m+cur.x] = maze[wall.y*m+wall.x] = maze[target.y*m+target.x] = EMPTY;
			cur = target;

			{
				glClearColor(1, 1, 1, 0);

				// очищаем экран
				glClear(GL_COLOR_BUFFER_BIT);

				const float size = 2.0f/n;
				vec2 origin = {-1, 1};

				for (int i = 0; i < n; ++i) {
					for (int j = 0; j < m; ++j) {
						int c = maze[i*m+j];
						switch (c) {
							case -1: glColor3f(1, 0, 0); break;
							case 0:  glColor3f(0, 1, 0); break;
							case 1:  glColor3f(0, 0, 0); break;
						}
						glBegin(GL_QUADS);
							glVertex2f(origin.x+(size*j),      origin.y-(size*i)-size);
							glVertex2f(origin.x+(size*j)+size, origin.y-(size*i)-size);
							glVertex2f(origin.x+(size*j)+size, origin.y-(size*i));
							glVertex2f(origin.x+(size*j),      origin.y-(size*i));
						glEnd();
					}
				}

				glfwSwapBuffers(window);
				Sleep(10);
			}
		} else if (history.size() != 0) {
			cur = history.top();
			history.pop();
		} else {
			cur = {unvisided_index%n, unvisided_index/n};
		}
	}

	return maze;
}

// код, который выполнится один раз до начала игрового цикла
void setup() {
	// инициализация генератора рандомных чисел
	srand(time(0));

	maze = generate_maze_matrix(MAZE_N, MAZE_M);
}


// наш код, который будет выполняться каждый кадр
void loop(double time) {

	// цвет очистки экрана - белый
	glClearColor(1, 1, 1, 0);

	// очищаем экран
	glClear(GL_COLOR_BUFFER_BIT);

	// рисуем:

	// матрицу лабиринта
	const float size = 2.0f/MAZE_N;
	vec2 origin = {-1, 1};

	for (int i = 0; i < MAZE_N; ++i) {
		for (int j = 0; j < MAZE_M; ++j) {
			int c = maze[i*MAZE_M+j];
			switch (c) {
				case -1: glColor3f(1, 0, 0); break;
				case 0:  glColor3f(0, 1, 0); break;
				case 1:  glColor3f(0, 0, 0); break;
			}
			glBegin(GL_QUADS);
				glVertex2f(origin.x+(size*j),      origin.y-(size*i)-size);
				glVertex2f(origin.x+(size*j)+size, origin.y-(size*i)-size);
				glVertex2f(origin.x+(size*j)+size, origin.y-(size*i));
				glVertex2f(origin.x+(size*j),      origin.y-(size*i));
			glEnd();
		}
	}
}



void setWindowFPS(GLFWwindow* window)
{
  // Measure speed
  double current_time = glfwGetTime();
  frame_number++;

  if (current_time - last_time >= 1.0){ // If last cout was more than 1 sec ago
    char title [20];
    title [19] = '\0';

    snprintf(title, 19, "[FPS: %d]", frame_number);

    glfwSetWindowTitle(window, title);

    frame_number = 0;
    last_time  = current_time;
  }
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
	window = glfwCreateWindow(WIDTH, HEIGHT, "Maze game", NULL, NULL);

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

	// наша инициализация
	setup();

	// пока окно не должно закрыться
	while (!glfwWindowShouldClose(window)) {

		// ставим фпсы
		setWindowFPS(window);

		// выполняем наш код
		double time = glfwGetTime();
		loop(time);

		// выводим на экран (это вместо glFlush)
		glfwSwapBuffers(window);

		// опрашиваем окно
		glfwPollEvents();
	}

	// закрываем glfw
	glfwTerminate();
}