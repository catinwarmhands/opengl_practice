#pragma once

#include "libs.cpp"
#include "utils.cpp"

#define GL_LOG_ERROR() {GLenum errorCode = glGetError(); if (errorCode != GL_NO_ERROR) std::cout << "OpenGL error code: " << errorCode; }

// скомпилировать шейдер из файлв, находящийся по указанному пути
GLuint compile_shader_from_file(const string& filePath)  {
	string temp = read_file(filePath);
	const char* fileData = temp.c_str();

	GLuint shaderID;

	if (ends_with(filePath, ".vert")) {
		shaderID = glCreateShader(GL_VERTEX_SHADER);
	} else {
		shaderID = glCreateShader(GL_FRAGMENT_SHADER);		
	}

	glShaderSource(shaderID, 1, &fileData, NULL);
	glCompileShader(shaderID);

	GLint err;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &err);

	if (err == GL_FALSE) {
		GLint length;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		string error(length, 0);
		glGetShaderInfoLog(shaderID, length, &length, &error[0]);
		if (length == 0)
			GL_LOG_ERROR();
		cout << "[compile_shader_from_file] Shader '" << filePath << "' compilation errors:\n" << &error[0] << endl;
		glDeleteShader(shaderID);
		exit(1);
		return 0;
	}
	cout << "[compile_shader_from_file] Shader '" << filePath << "' compilation OK" << endl;
	return shaderID;
}

// слинковать вершинный и фрагментный шейдер в программу
GLuint link_shader_program(GLuint vertexShader, GLuint fragmentShader) {
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);

	glLinkProgram(programID);

	GLint err = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &err);

	if (err == GL_FALSE) {
		GLint length = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
		string error(length, 0);
		glGetProgramInfoLog(programID, length, &length, &error[0]);
		if (length == 0)
			GL_LOG_ERROR();
		cout << "[link_shader_program] ShaderProgram linking errors:\n" << &error[0] << endl;
		glDetachShader(programID, vertexShader);
		glDetachShader(programID, fragmentShader);
		glDeleteProgram(programID);
		exit(1);
		return 0;
	}
	// cout << "[link_shader_program] ShaderProgram linking OK" << endl;

	glValidateProgram(programID);

	glGetProgramiv(programID, GL_VALIDATE_STATUS, &err);
	if (err == GL_FALSE) {
		GLint length = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
		string error(length, 0);
		glGetProgramInfoLog(programID, length, &length, &error[0]);
		if (length == 0)
			GL_LOG_ERROR();
		cout << "[link_shader_program] ShaderProgram validation errors:\n" << &error[0] << endl;
		glDetachShader(programID, vertexShader);
		glDetachShader(programID, fragmentShader);
		glDeleteProgram(programID);
		exit(1);
		return 0;
	}
	cout << "[link_shader_program] ShaderProgram validation OK" << endl;
	glDetachShader(programID, vertexShader);
	glDetachShader(programID, fragmentShader);
	return programID;
}