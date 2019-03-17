#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;

#include "../engine/engine.cpp"

namespace Globals {
	String geom;
	u64 shader;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

vec2 cursorPos;

const int WIDTH  = 640;
const int HEIGHT = 480;

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	cursorPos.x = (float)x;
	cursorPos.y = (float)y;
}

vec2 fix_cursor_pos(vec2 pos) {
	return vec2(pos.x, HEIGHT - pos.y);
}

int main() {
	varprint(PATH);
	if (!glfwInit()) {
		logger.fatal("[GLFW] failed to init");
	}

	defer_qexit(glfwTerminate());

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
	if (!window) {
		logger.fatal("[GLFW] failed to open window");
	}
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwMakeContextCurrent(window);
	
	auto glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		logger.fatal("[GLEW] failed to init: {}", glewGetErrorString(glewErr));
	}


	ShaderProgram shader1 = compile_shader_program_from_files(PATH"shaders\\basic.vert", PATH"shaders\\basic.frag");
	ShaderProgram shader2 = compile_shader_program_from_files(PATH"shaders\\basic.vert", PATH"shaders\\rainbow.frag");
	ShaderProgram shader3 = compile_shader_program_from_files(PATH"shaders\\basic.vert", PATH"shaders\\fractal.frag");

	//BUG hotloading not working in release build
	Hotloader hotloader;
	using namespace Globals;
	hotloader.attach("Misc",  MAKE_LVALUE(geom));
	hotloader.attach("Misc",  MAKE_LVALUE2("shader", Globals::shader));
	hotloader.attach(PATH"All.variables");
	hotloader.loadVariables(PATH"All.variables");
	hotloader.attach(shader1);
	hotloader.attach(shader2);
	hotloader.attach(shader3);


	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f,  0.5f, 0.0f
	};

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);  

	while (!glfwWindowShouldClose(window)) {

		if (hotloader.needsReload()) {
			// logger.info("Reloading {} entines", hotloader.updates.size());
			hotloader.reload();

			//IMPROVEMENT add method to hotloader to check what kind of resourse was updated
			// use_program(shader);
		}
		ShaderProgram activeProgram;
		switch (Globals::shader) {
			case 1: activeProgram = shader1; break;
			case 2: activeProgram = shader2; break;
			case 3: activeProgram = shader3; break;
		}

		use_program(activeProgram);


		glClear(GL_COLOR_BUFFER_BIT);

		glUniform3fv(glGetUniformLocation(activeProgram.programID, "iResolution"), 1, value_ptr(vec3(640.f, 480.f, 1.f)));
		glUniform2fv(glGetUniformLocation(activeProgram.programID, "iMouse"),      1, value_ptr(fix_cursor_pos(cursorPos)));
		glUniform1f( glGetUniformLocation(activeProgram.programID, "iGlobalTime"),    (float)glfwGetTime());

		// if (geom == "triangle") {
		// 	glBegin(GL_POLYGON);
		// 		glVertex3f(-1, -1, 0);
		// 		glVertex3f(1, -1, 0);
		// 		glVertex3f(0, 1, 0);
		// 	glEnd();
		// } else if (geom == "square") {
		// 	glBegin(GL_QUADS);
		// 		glVertex2f(-1.f, -1.f);
		// 		glVertex2f( 1.f, -1.f);
		// 		glVertex2f( 1.f,  1.f);
		// 		glVertex2f(-1.f,  1.f);
		// 	glEnd();
		// }

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
	QEXIT;
}