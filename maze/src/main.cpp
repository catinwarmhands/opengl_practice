#include "libs.cpp"
#include "globals.cpp"
#include "utils.cpp"
#include "shader.cpp"
#include "texture.cpp"
#include "model.cpp"
#include "maze_generator.cpp"

// загрузка ресурсов
void load_resources() {
	Mesh cubeMesh = read_mesh(rootPath+"models\\cube.obj");
	cubeModel = make_model(&cubeMesh);

	Mesh playerMesh = read_mesh(rootPath+"models\\cube.obj");
	for (int i = 0; i < playerMesh.vertexPositions.size(); ++i)
		playerMesh.vertexPositions[i] *= player.size;
	playerModel = make_model(&playerMesh);

	Mesh coinMesh = read_mesh(rootPath+"models\\coin.obj");
	for (int i = 0; i < coinMesh.vertexPositions.size(); ++i)
		coinMesh.vertexPositions[i] *= 2;
	coinModel = make_model(&coinMesh);

	// компилируем шейдеры
	vertexShaders.push_back(compile_shader_from_file(rootPath+"shaders\\basic.vert"));
	vertexShaders.push_back(compile_shader_from_file(rootPath+"shaders\\wavy.vert"));
	fragmentShaders.push_back(compile_shader_from_file(rootPath+"shaders\\basic.frag"));
	fragmentShaders.push_back(compile_shader_from_file(rootPath+"shaders\\gold.frag"));
	shaderPrograms.push_back(link_shader_program(vertexShaders[0], fragmentShaders[0]));
	shaderPrograms.push_back(link_shader_program(vertexShaders[1], fragmentShaders[0]));
	shaderPrograms.push_back(link_shader_program(vertexShaders[0], fragmentShaders[1]));
	currentShader = shaderPrograms[0];

	// грузим текстуры
	cat = load_texture_from_file(rootPath+"textures\\cat.png");
	grass = load_texture_from_file(rootPath+"textures\\grass.png");
	cobblestone = load_texture_from_file(rootPath+"textures\\cobblestone.png", GL_REPEAT, GL_REPEAT);
	lev = load_texture_from_file(rootPath+"textures\\lev.jpg");
	coin = load_texture_from_file(rootPath+"textures\\coin.png");

	// загружаем шрифты
	fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
	if (fs == NULL) {
		cout << "[font loading] Could not create stash" << endl;
		exit(1);
	}

	fontNormal = fonsAddFont(fs, "sans", (rootPath+"fonts\\DroidSerif-Regular.ttf").c_str());
	if (fontNormal == FONS_INVALID) {
		cout << "[font loading] Could not add font normal" << endl;
		exit(1);
	}
}

// поставить камеру в положение по умолчанию
void set_default_camera() {
	if (firstPersonMode) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		input.cursorPosition = {x,y};
		camera.yaw = -90;
		camera.pitch = -45;
	}
}

// установить сообщение со счетом
void set_score_message() {
	ostringstream oss;
	oss << "Монеты: " << score << "/" << coinsPositions.size();
	scoreMessage = oss.str();
}

// установить сообщение с временем
void set_time_message() {
	ostringstream oss;
	oss.precision(2);
	oss << "Время: " << fixed << currentTime << "с";
	timeMessage = oss.str();
}

// установить сообщение с лучшим временем
void set_best_time_message() {
	ostringstream oss;
	oss.precision(2);
	if (winTime < bestTime || bestTime < 0) {
		oss << "Новый рекорд: " << fixed << winTime << "с";
	} else {
		oss << "Время: " << fixed << winTime << "с (рекорд: " << fixed << bestTime << "с)";
	}
	bestTimeMessage = oss.str();
}

// установить матрицу проекции
void set_projection() {
	if (firstPersonMode) {
		projection = perspective(radians(camera.fov), (float)frameBufferSize.x / (float)frameBufferSize.y, 0.1f, 100.0f);
	} else {
		const float aspect = (float)frameBufferSize.y / (float)frameBufferSize.x;
		projection = ortho(-camera.scale, camera.scale, -camera.scale * aspect, camera.scale * aspect, 0.1f, 100.0f);
	}
}

// перезапустить игру
void restart_game() {
	mazeSize = {linearRand(11, 31), linearRand(11, 31)};
	mazeSize.x = mazeSize.x + (1-mazeSize.x%2);
	mazeSize.y = mazeSize.y + (1-mazeSize.y%2);

	mazeMatrix = generate_maze_matrix(mazeSize.x, mazeSize.y);
	mazeMesh = make_mesh_from_maze_matrix(mazeMatrix, mazeSize.x, mazeSize.y);
	mazeModel = make_model(&mazeMesh);

	cubesPositions = generate_cubes_positions(mazeSize.x, mazeSize.y);
	coinsPositions = get_coins_positions_from_maze_matrix(mazeMatrix, mazeSize.x, mazeSize.y);

	groundMesh = generate_ground_mesh(mazeSize.x, mazeSize.y);
	groundModel = make_model(&groundMesh);

	player.position = get_player_position_from_maze_matrix(mazeMatrix, mazeSize.x, mazeSize.y);
	score = 0;
	isWin = false;
	set_score_message();
	set_projection();

	previousTime = currentTime = 0;
	glfwSetTime(0);
}

// код, который выполнится один раз до начала игрового цикла
void setup() {
	// инициализация генератора рандомных чисел
	srand(time(0));

	glClearColor(0.5, 0.6, 0.7, 0);

	load_resources();
	restart_game();
	set_default_camera();
}

// вернет true, если игрок пересекается со стенкой
// алгоритм AABB
bool check_player_collision(vec3 pos) {
	pos.x += player.size/2;
	pos.z += player.size/2;
	vec3 size(1, 0.5, 1);
	vec2 origin = {0, 0};
	vec2 cur;
	for (int i = 0; i < mazeSize.x; ++i) {
		cur.y = origin.y - size.z*i;
		for (int j = 0; j < mazeSize.y; ++j) {
			cur.x = origin.x + size.x*j;
			int c = mazeMatrix[i*mazeSize.y+j];
			if (c == 1 && distance({pos.x, pos.z}, cur) <= 4.0f) {
				if (!(pos.x + player.size < cur.x || pos.x > cur.x + size.x || pos.z + player.size < cur.y || pos.z > cur.y + size.z))
					return true;
			}
		}
	}
	return false;
}

// корректирует новую позицию игрока
vec3 do_player_collision(vec3 pos) {
	if (!check_player_collision(pos) || noclipMode)
		return pos;

	vec3 t = {player.position.x, pos.y, pos.z};
	if (!check_player_collision(t))
		return t;

	t = {pos.x, pos.y, player.position.z};
	if (!check_player_collision(t))
		return t;
	return player.position;
}

// пересчитать направляющий вектор камеры
void update_camera_front() {
	vec3 front;
	front.x = cos(radians(camera.pitch)) * cos(radians(camera.yaw));
	front.y = sin(radians(camera.pitch));
	front.z = cos(radians(camera.pitch)) * sin(radians(camera.yaw));
	camera.front = normalize(front);
}

// обработка инпута
void process_input() {
	// скролл
	if (input.hasScrollOffset) {
		camera.fov   -= input.scrollOffset.y;
		camera.scale -= input.scrollOffset.y;
		if (camera.fov < minFov)
			camera.fov = minFov;
		if (camera.scale < minScale)
			camera.scale = minScale;
		input.scrollOffset.x = 0;
		input.scrollOffset.y = 0;
		input.hasScrollOffset = false;
		set_projection();
	}

	// вращение камеры
	if (input.hasCursorOffset) {
		camera.yaw   += input.cursorOffset.x;
		camera.pitch += input.cursorOffset.y;
		input.cursorOffset.x = 0;
		input.cursorOffset.y = 0;
		input.hasCursorOffset = false;
		if (camera.pitch > 89.0f)
			camera.pitch =  89.0f;
		if (camera.pitch < -89.0f)
			camera.pitch = -89.0f;
	}

	// камеру нужно обновлять именно в этом месте
	update_camera_front();

	// движение игрока (и камеры за ним)
	float speed = player.speed*dt;
	vec3 newPosition = player.position;
	vec3 cameraOffset;
	if (firstPersonMode) {
		if (input.keys[GLFW_KEY_W])
			newPosition -= speed * normalize(cross(cross(camera.front, camera.up), camera.up));
		if (input.keys[GLFW_KEY_S])
			newPosition += speed * normalize(cross(cross(camera.front, camera.up), camera.up));
		if (input.keys[GLFW_KEY_A])
			newPosition -= normalize(cross(camera.front, camera.up)) * speed;
		if (input.keys[GLFW_KEY_D])
			newPosition += normalize(cross(camera.front, camera.up)) * speed;

		cameraOffset = - 3.0f*camera.front+vec3(0,0.7,0);
	} else {
		if (input.keys[GLFW_KEY_W])
			newPosition.x -= speed;
		if (input.keys[GLFW_KEY_S])
			newPosition.x += speed;
		if (input.keys[GLFW_KEY_A])
			newPosition.z += speed;
		if (input.keys[GLFW_KEY_D])
			newPosition.z -= speed;

		cameraOffset = vec3(0,10,0);
		camera.yaw = 180;
		camera.pitch = -89;
	}

	newPosition.y = -player.size/2;
	player.position = do_player_collision(newPosition);
	camera.position = player.position + cameraOffset;

	// закрытие окна
	if (input.keys[GLFW_KEY_ESCAPE]) {
		input.keys[GLFW_KEY_ESCAPE] = false;
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	// изменение вида камеры
	if (input.keys[GLFW_KEY_Q]) {
		input.keys[GLFW_KEY_Q] = false;
		firstPersonMode = !firstPersonMode;
		set_projection();
	}

	// рестарт
	if (input.keys[GLFW_KEY_R]) {
		input.keys[GLFW_KEY_R] = false;
		restart_game();
	}

	// ноклип
	if (input.keys[GLFW_KEY_N]) {
		input.keys[GLFW_KEY_N] = false;
		noclipMode = !noclipMode;
	}

	// отрисовка сетки
	if (input.keys[GLFW_KEY_F]) {
		input.keys[GLFW_KEY_F] = false;
		wireframeMode = !wireframeMode;
		if (wireframeMode) 
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// захват мышки
	if (input.keys[GLFW_KEY_M]) {
		input.keys[GLFW_KEY_M] = false;
		mouseLock = !mouseLock;
		if (mouseLock) 
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	//фуллскрин
	if (input.keys[GLFW_KEY_F11]) {
		input.keys[GLFW_KEY_F11] = false;
		fullScreen = !fullScreen;
		const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		if (fullScreen)
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		else
			glfwSetWindowMonitor(window, 0, 100, 100, defaultWindowSize.x, defaultWindowSize.y, mode->refreshRate);
	}
}

// отрендерить сцену
void render_scene() {
	//очищаем экран
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 view;
	view = lookAt(camera.position, camera.position + camera.front, camera.up);

	currentShader = shaderPrograms[0];
	glUseProgram(currentShader);

	glUniformMatrix4fv(glGetUniformLocation(currentShader, "view"),       1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(currentShader, "projection"), 1, GL_FALSE, value_ptr(projection));
	glUniform1f(       glGetUniformLocation(currentShader, "time"), (GLfloat)currentTime);

	glEnable(GL_DEPTH_TEST);

	//cubes
	{
		glBindTexture(GL_TEXTURE_2D, cat);
		glBindVertexArray(cubeModel.VAO);
		for(GLuint i = 0; i < cubesPositions.size(); i++)
		{
			mat4 model;
			model = translate(model, cubesPositions[i]);
			GLfloat angle = radians((GLfloat)currentTime*100.0f+i*100);
			model = rotate(model, angle, vec3(1.0f, 0.3f, 0.5f));
			glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, value_ptr(model));

			glDrawElements(GL_TRIANGLES, cubeModel.indicesCount, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);
	}

	// ground
	{
		glBindTexture(GL_TEXTURE_2D, cobblestone);
		glBindVertexArray(groundModel.VAO);
		mat4 model;
		glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, value_ptr(model));
		glDrawElements(GL_TRIANGLES, groundModel.indicesCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	// maze
	{
		currentShader = shaderPrograms[1];
		glUseProgram(currentShader);
		glBindTexture(GL_TEXTURE_2D, grass);
		glBindVertexArray(mazeModel.VAO);
		mat4 model;
		glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(currentShader, "view"),       1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(currentShader, "projection"), 1, GL_FALSE, value_ptr(projection));
		glUniform1f(       glGetUniformLocation(currentShader, "time"), (GLfloat)currentTime);

		glDrawElements(GL_TRIANGLES, mazeModel.indicesCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		currentShader = shaderPrograms[0];
		glUseProgram(currentShader);
	}

	// coins
	{
		currentShader = shaderPrograms[2];
		glUseProgram(currentShader);
		glBindTexture(GL_TEXTURE_2D, coin);
		glBindVertexArray(coinModel.VAO);
		glUniformMatrix4fv(glGetUniformLocation(currentShader, "view"),       1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(currentShader, "projection"), 1, GL_FALSE, value_ptr(projection));
		glUniform1f(       glGetUniformLocation(currentShader, "time"), (GLfloat)currentTime);
		for(GLuint i = 0; i < coinsPositions.size(); i++)
		{
			mat4 model;
			model = translate(model, coinsPositions[i]);
			model = rotate(model, radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
			GLfloat angle = radians((GLfloat)currentTime*100.0f+i*100);
			model = rotate(model, angle, vec3(0.0f, 0.0f, 1.0f));
			glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, value_ptr(model));

			glDrawElements(GL_TRIANGLES, coinModel.indicesCount, GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);
		currentShader = shaderPrograms[0];
		glUseProgram(currentShader);
	}

	// player
	// if (!firstPersonMode)
	{
		glBindTexture(GL_TEXTURE_2D, lev);
		glBindVertexArray(playerModel.VAO);
		mat4 model;
		model = translate(model, player.position);
		model = rotate(model, radians(player.orientation), vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, value_ptr(model));
		glDrawElements(GL_TRIANGLES, playerModel.indicesCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
}

// отрендерить hud
void render_text() {
	// понятия не имею зачем это нужно, но без этого не работает
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,frameBufferSize.x,frameBufferSize.y,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor4ub(255,255,255,255);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	fonsClearState(fs);
	float dx = 10, dy = 10;

	if (isWin) {
		fonsSetFont(fs, fontNormal);
		fonsSetSize(fs, 124.0f);
		fonsSetColor(fs, white);
		fonsSetAlign(fs, FONS_ALIGN_CENTER | FONS_ALIGN_BOTTOM);
		fonsDrawText(fs, frameBufferSize.x/2, frameBufferSize.y/2, winMessage.c_str(), NULL);

		fonsSetSize(fs, 48.0f);
		fonsSetAlign(fs, FONS_ALIGN_CENTER | FONS_ALIGN_TOP);
		fonsDrawText(fs, frameBufferSize.x/2, frameBufferSize.y/2, bestTimeMessage.c_str(), NULL);
	} else {
		fonsSetSize(fs, 36.0f);
		fonsSetAlign(fs, FONS_ALIGN_LEFT | FONS_ALIGN_TOP);
		fonsDrawText(fs, dx, dy, scoreMessage.c_str(), NULL);
		fonsSetAlign(fs, FONS_ALIGN_RIGHT | FONS_ALIGN_TOP);
		fonsDrawText(fs, frameBufferSize.x-dx, dy, timeMessage.c_str(), NULL);
	}

	if (noclipMode) {
		fonsSetSize(fs, 28.0f);
		fonsSetAlign(fs, FONS_ALIGN_CENTER | FONS_ALIGN_TOP);
		fonsSetColor(fs, red);
		fonsDrawText(fs, frameBufferSize.x/2, dy, noclipMessage.c_str(), NULL);
	}

	glEnable(GL_DEPTH_TEST);
}

// проверяем, не подобрал ли игрок монету
void check_coins() {
	for (int i = 0; i < coinsPositions.size(); ++i) {
		if (distance(player.position, coinsPositions[i]) < player.size) {
			// если подобрал, то увеличиваем счет и убираем её с глаз долой
			coinsPositions[i] = {0,-999,0};
			++score;
			set_score_message();
		}
	}
}

// код, который будет выполняться каждый кадр
void loop() {
	process_input();
	check_coins();

	if (score >= coinsPositions.size() && !isWin) {
		isWin = true;
		winTime = currentTime;
		set_best_time_message();
		if (winTime < bestTime || bestTime < 0) 
			bestTime = winTime;
	}

	if (!isWin)
		set_time_message();

	render_scene();
	render_text();
}

// вывести в шапку окна dt и приблизительное количество фпс
// по умолчанию выводит каждые 100 кадров
void set_window_fps(int every=100) {
	if (frameNumber % every == 0) {
		char title [50];
		title [49] = '\0';
		int fps = 1.0/dt;
		snprintf(title, 49, "FPS: %d, dt: %f", fps, dt);
		glfwSetWindowTitle(window, title);
	}
}

// коллбеки на события
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		input.keys[key] = true;
	else if(action == GLFW_RELEASE)
		input.keys[key] = false;
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	windowSize.x = width;
	windowSize.y = height;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	frameBufferSize.x = width;
	frameBufferSize.y = height;
	set_projection();
    glViewport(0, 0, width, height);
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	if (firstPersonMode) {
		input.cursorOffset.x = (x - input.cursorPosition.x)*mouseSensitivity;
		input.cursorOffset.y = (input.cursorPosition.y - y)*mouseSensitivity;
		input.cursorPosition.x = x;
		input.cursorPosition.y = y;
		input.hasCursorOffset = true;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	input.scrollOffset.x = xoffset;
	input.scrollOffset.y = yoffset;
	input.hasScrollOffset = true;
}

int main() {
	// узнаем корневую директорию (для доступа к ресурсам)
	rootPath = get_root_path();

	// запускаем glfw
	if (!glfwInit()) {
		// если не запускается, то выключаемся
		cout << "[GLFW] failed to init" << endl;
		exit(1);
	}

	// создаем окно
	window = glfwCreateWindow(windowSize.x, windowSize.y, "Maze game", NULL, NULL);

	// если не создалось
	if (!window) {
		// то отключаемся
		cout << "[GLFW] failed to open window" << endl;
		glfwTerminate();
		exit(1);
	}

	// узнаем размер фреймбуфера
	glfwGetFramebufferSize(window, &frameBufferSize.x, &frameBufferSize.y);

	// будем рисовать в это окно
	glfwMakeContextCurrent(window);

	// запускаем glew
	GLenum glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		// если не запускается, то отключаемся
		cout << "[GLEW] failed to init: " << glewGetErrorString(glewErr) << endl;
		glfwTerminate();
		exit(1);
	}

	// вешаем коллбеки
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// лочим мышку
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
