#include "libs.cpp"
#include "globals.cpp"
#include "utils.cpp"
#include "shader.cpp"
#include "texture.cpp"
#include "model.cpp"


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

	glClearColor(0.5, 0.6, 0.7, 0);
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);


	Mesh mesh;
	mesh.vertexPositions = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};
	mesh.indices = {
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35
	}; 
	mesh.textureCoords = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f
	};

	cube = make_model(&mesh);

	cubePositions = {
		vec3( 0.0f,  0.0f,  0.0f), 
		vec3( 2.0f,  5.0f, -15.0f), 
		vec3(-1.5f, -2.2f, -2.5f),  
		vec3(-3.8f, -2.0f, -12.3f),  
		vec3( 2.4f, -0.4f, -3.5f),  
		vec3(-1.7f,  3.0f, -7.5f),  
		vec3( 1.3f, -2.0f, -2.5f),  
		vec3( 1.5f,  2.0f, -2.5f), 
		vec3( 1.5f,  0.2f, -1.5f), 
		vec3(-1.3f,  1.0f, -1.5f)  
	};


	// компилируем шейдеры
	vertexShaders.push_back(compile_shader_from_file(rootPath+"shaders\\basic.vert"));
	fragmentShaders.push_back(compile_shader_from_file(rootPath+"shaders\\basic.frag"));

	shaderPrograms.push_back(link_shader_program(vertexShaders[0], fragmentShaders[0]));

	// грузим текстуры
	textures.push_back(load_texture_from_file(rootPath+"textures\\harry.jpg"));
	textures.push_back(load_texture_from_file(rootPath+"textures\\cat.png"));
	textures.push_back(load_texture_from_file(rootPath+"textures\\mad.jpg"));
	textures.push_back(load_texture_from_file(rootPath+"textures\\pearl.png"));
	// textures.push_back(load_texture_from_file(rootPath+"textures\\grass_side.png"));
	// textures.push_back(load_texture_from_file(rootPath+"textures\\grass_top.png"));
	// textures.push_back(load_texture_from_file(rootPath+"textures\\cobblestone.png"));

	modeN = textures.size();

	// генерируем лабиринт
	maze = generate_maze_matrix(MAZE_N, MAZE_M);

	view = translate(view, vec3(0.0f, 0.0f, -3.0f));
	projection = perspective(radians(45.0f), (float)frameBufferSize.x/(float)frameBufferSize.y, 0.1f, 100.0f);

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	input.cursorPosition = {x,y};

	camera.yaw = -90;
}

void do_camera_movement() {
	camera.yaw   += input.cursorOffset.x;
	camera.pitch += input.cursorOffset.y;
	input.cursorOffset.x = 0;
	input.cursorOffset.y = 0;

	if (camera.pitch > 89.0f)
		camera.pitch =  89.0f;
	if (camera.pitch < -89.0f)
		camera.pitch = -89.0f;

	vec3 front;
	front.x = cos(radians(camera.pitch)) * cos(radians(camera.yaw));
	front.y = sin(radians(camera.pitch));
	front.z = cos(radians(camera.pitch)) * sin(radians(camera.yaw));
	camera.front = normalize(front);

	float speed = cameraSpeed*dt;

	if (input.keys[GLFW_KEY_W])
		camera.position += speed * camera.front;
	if (input.keys[GLFW_KEY_S])
		camera.position -= speed * camera.front;
	if (input.keys[GLFW_KEY_A])
		camera.position -= normalize(cross(camera.front, camera.up)) * speed;
	if (input.keys[GLFW_KEY_D])
		camera.position += normalize(cross(camera.front, camera.up)) * speed; 
	if (input.keys[GLFW_KEY_SPACE])
		camera.position += speed * camera.up; 
	if (input.keys[GLFW_KEY_LEFT_CONTROL])
		camera.position -= speed * camera.up; 
}


// код, который будет выполняться каждый кадр
void loop() {

	do_camera_movement();
	// очищаем экран
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw_maze_matrix(maze);

	mat4 model;
	model = rotate(model, radians((GLfloat)currentTime*55.0f), vec3(0.5f, 1.0f, 0.0f));

	
	glUseProgram(shaderPrograms[0]);

	// GLfloat radius = 10.0f;
	mat4 view;
	view = lookAt(camera.position, camera.position + camera.front, camera.up);

	glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[0], "view"),       1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[0], "projection"), 1, GL_FALSE, value_ptr(projection));

	glBindTexture(GL_TEXTURE_2D, textures[mode-1]);
	glBindVertexArray(cube.VAO);
	for(GLuint i = 0; i < 10; i++)
	{
		mat4 model;
		model = translate(model, cubePositions[i]);
		GLfloat angle = radians((GLfloat)currentTime*20.0f*(i+1));
		model = rotate(model, angle, vec3(1.0f, 0.3f, 0.5f));
		glUniformMatrix4fv(glGetUniformLocation(shaderPrograms[0], "model"),      1, GL_FALSE, value_ptr(model));

		glDrawElements(GL_TRIANGLES, cube.indicesCount, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
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
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    	wireframeMode = !wireframeMode;
    	if (wireframeMode) 
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (key == GLFW_KEY_M && action == GLFW_PRESS) {
    	mouseLock = !mouseLock;
    	if (mouseLock) 
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
    	fullScreen = !fullScreen;
		const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    	if (fullScreen) {
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
    	}
		else {
			glfwSetWindowMonitor(window, 0, 100, 100, defaultWindowSize.x, defaultWindowSize.y, mode->refreshRate);
		}
    }
    

	if (action == GLFW_PRESS)
		input.keys[key] = true;
	else if(action == GLFW_RELEASE)
		input.keys[key] = false;
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	windowSize.x = width;
	windowSize.y = height;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	frameBufferSize.x = width;
	frameBufferSize.y = height;
	projection = perspective(radians(45.0f), (float)frameBufferSize.x/(float)frameBufferSize.y, 0.1f, 100.0f);
    glViewport(0, 0, width, height);
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	input.cursorOffset.x = (x - input.cursorPosition.x)*mouseSensitivity;
	input.cursorOffset.y = (input.cursorPosition.y - y)*mouseSensitivity;
	input.cursorPosition.x = x;
	input.cursorPosition.y = y;
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
	window = glfwCreateWindow(windowSize.x, windowSize.y, "Maze game", NULL, NULL);

	// если не создалось
	if (!window) {
		// то отключаемся
		cout << "[GLFW] failed to open window" << endl;
		glfwTerminate();
		return 1;
	}

	// узнаем размер фреймбуфера
	glfwGetFramebufferSize(window, &frameBufferSize.x, &frameBufferSize.y);

	// будем рисовать в это окно
	glfwMakeContextCurrent(window);

	// коллбеки
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// запускаем glew
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		// если не запускается, то отключаемся
		cout << "[GLEW] failed to init: " << glewGetErrorString(glewErr) << endl;
		glfwTerminate();
		return 1;
	}

	// glViewport(0, 0, resolution.x, resolution.y);
	glEnable(GL_DEPTH_TEST);

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

		// увеличиваем номер текущего кадра
		frameNumber++;

		// выводим на экран (это вместо glFlush)
		glfwSwapBuffers(window);
	}

	// закрываем glfw
	glfwTerminate();
}