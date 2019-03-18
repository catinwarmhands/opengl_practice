#include "libs.cpp"
#include "globals.cpp"
#include "utils.cpp"
#include "shader.cpp"

void draw_maze_matrix(int* maze) {
	const float size = 2.0f/MAZE_N;
	vec2 origin = {-1, 1};
	vec2 cur;
	for (int i = 0; i < MAZE_N; ++i) {
		cur.y = origin.y - size*i;
		for (int j = 0; j < MAZE_M; ++j) {
			cur.x = origin.x + size*j;
			int c = maze[i*MAZE_M+j];
			switch (c) {
				case -1: glColor3f(1, 0, 0); break;
				case 0:  glColor3f(0, 1, 0); break;
				case 1:  glColor3f(0, 0, 0); break;
			}
			vec2 o = {origin.x+(size*j), origin.y-(size*i)};
			glBegin(GL_QUADS);
				glVertex2f(cur.x,      cur.y-size);
				glVertex2f(cur.x+size, cur.y-size);
				glVertex2f(cur.x+size, cur.y);
				glVertex2f(cur.x,      cur.y);
			glEnd();
		}
	}
}

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

	stack<ivec2> history;

	ivec2 cur = {linearRand(1, n-2), linearRand(1, m-2)};
	cur.x = cur.x + (1-cur.x%2);
	cur.y = cur.y + (1-cur.y%2);

	while (true) {
		int unvisidedIndex = -1;
		for (int i = 0; i < n*m; ++i)
			if (maze[i] == UNVISITED)
				unvisidedIndex = i;

		if (unvisidedIndex == -1)
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
			if (0 <= v.x && v.x < m && 0 <= v.y && v.y < n && maze[v.y*m + v.x] == UNVISITED)
				neighbours.push_back(v);
		}

		if (neighbours.size() != 0) {
			history.push(cur);
			ivec2 target = neighbours[linearRand(0, (int)neighbours.size()-1)];
			ivec2 wall = cur + ivec2(-sgn(cur.x-target.x), -sgn(cur.y-target.y));
			maze[cur.y*m+cur.x] = maze[wall.y*m+wall.x] = maze[target.y*m+target.x] = EMPTY;
			cur = target;

			// debug draw
			#if 0
				glfwPollEvents();
				if (glfwWindowShouldClose(window))
					break;
				glClear(GL_COLOR_BUFFER_BIT);
				draw_maze_matrix(maze);
				glfwSwapBuffers(window);
				Sleep(10);
			#endif
		} else if (history.size() != 0) {
			cur = history.top();
			history.pop();
		} else {
			cur = {unvisidedIndex%n, unvisidedIndex/n};
		}
	}

	return maze;
}

// код, который выполнится один раз до начала игрового цикла
void setup() {
	// инициализация генератора рандомных чисел
	srand(time(0));

	// компилируем шейдеры
	fragmentShaders.push_back(compile_shader_from_file(rootPath+"shaders\\basic.frag"));
	fragmentShaders.push_back(compile_shader_from_file(rootPath+"shaders\\rainbow.frag"));
	fragmentShaders.push_back(compile_shader_from_file(rootPath+"shaders\\fractal.frag"));

	vertexShaders.push_back(compile_shader_from_file(rootPath+"shaders\\basic.vert"));
	vertexShaders.push_back(compile_shader_from_file(rootPath+"shaders\\wobble.vert"));

	int i = 0;
	shaderPrograms.push_back(link_shader_program(vertexShaders[i], fragmentShaders[0]));
	shaderPrograms.push_back(link_shader_program(vertexShaders[i], fragmentShaders[1]));
	shaderPrograms.push_back(link_shader_program(vertexShaders[i], fragmentShaders[2]));
	modeN = shaderPrograms.size();

	// генерируем лабиринт
	maze = generate_maze_matrix(MAZE_N, MAZE_M);

	GLint nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
}


// код, который будет выполняться каждый кадр
void loop() {
	// очищаем экран
	glClear(GL_COLOR_BUFFER_BIT);

	// draw_maze_matrix(maze);

	glUseProgram(shaderPrograms[mode-1]);
	glUniform2iv(glGetUniformLocation(shaderPrograms[mode-1], "iResolution"), 1, value_ptr(resolution));
	glUniform2fv(glGetUniformLocation(shaderPrograms[mode-1], "iMouse"),      1, value_ptr(cursorPosition));
	glUniform1f( glGetUniformLocation(shaderPrograms[mode-1], "iGlobalTime"),    (float)glfwGetTime());

	glBegin(GL_POLYGON);
		glVertex3f(-0.5, -0.5, 0);
		glVertex3f( 0.5, -0.5, 0);
		glVertex3f( 0.0,  0.5, 0);
	glEnd();
}

void set_window_fps(int every=100)
{
	if (frameNumber % every == 0) {
		char title [50];
		title [49] = '\0';
		int fps = 1.0/dt;
		snprintf(title, 49, "FPS: %d, dt: %f", fps, dt);
		glfwSetWindowTitle(window, title);
	}
}

// функция, обрабатывающая нажатие на кнопки
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) //если нажали на ESC
        glfwSetWindowShouldClose(window, GLFW_TRUE); //то окно надо закрыть
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
    	mode += 1;
    	if (mode > modeN)
    		mode = 1;
    }
    // if (key == GLFW_KEY_A)
    //     glRotatef(1, 0.0, 0.0, 1.0);
    // if (key == GLFW_KEY_D)
    //     glRotatef(-1, 0.0, 0.0, 1.0);
    // if (key == GLFW_KEY_W)
    //     glTranslatef(0.0, -0.01, 0.0);
    // if (key == GLFW_KEY_S)
    //     glTranslatef(0.0, 0.01, 0.0);
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	cursorPosition.x = x;
	cursorPosition.y = resolution.y-y;
}

int main() {
	// узнаем корневую директорию (для доступа к ресурсам)
	rootPath = get_root_path();

	// запускаем glfw
	if (!glfwInit()) {
		// если не запускается, то выключаемся
		cout << "[GLFW] failed to init" << endl;
		return 1;
	}

	// создаем окно
	window = glfwCreateWindow(resolution.x, resolution.y, "Maze game", NULL, NULL);

	// если не создалось
	if (!window) {
		// то отключаемся
		cout << "[GLFW] failed to open window" << endl;
		glfwTerminate();
		return 1;
	}

	// будем рисовать в это окно
	glfwMakeContextCurrent(window);

	// коллбеки для клавиатуры и мыши
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);


	// запускаем glew
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		// если не запускается, то отключаемся
		cout << "[GLEW] failed to init: " << glewGetErrorString(glewErr) << endl;
		glfwTerminate();
		return 1;
	}

	// glViewport(0, 0, resolution.x, resolution.y);

	// цвет очистки экрана - белый
	// glClearColor(1, 1, 1, 0);


	// наша инициализация
	setup();

	// пока окно не должно закрыться
	while (!glfwWindowShouldClose(window)) {
		previousTime = currentTime;
		currentTime = glfwGetTime();
		dt = currentTime - previousTime;

		// опрашиваем окно
		glfwPollEvents();

		// ставим фпсы
		set_window_fps(500);

		// выполняем наш код
		loop();

		// glRotatef(camera.orientation, 0.0, 1.0, 0.0);
		// glTranslatef(0.0, 0.0, -5.0);

		// увеличиваем номер текущего кадра
		frameNumber++;

		// выводим на экран (это вместо glFlush)
		glfwSwapBuffers(window);
	}

	// закрываем glfw
	glfwTerminate();
}