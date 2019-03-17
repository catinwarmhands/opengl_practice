#pragma once

#include "utils.cpp"
#include "logger.cpp"

#define GL_LOG_ERROR() {GLenum errorCode = glGetError(); if (errorCode != GL_NO_ERROR) logger.error("OpenGL error code: {}",errorCode); }

struct ShaderProgram {
	GLuint programID;
	String vertPath;
	String fragPath;
	bool valid;
	//..
};

ShaderProgram compile_shader_program_from_files(const String& vertPath, const String& fragPath) {
	ShaderProgram result;
	result.vertPath = vertPath;
	result.fragPath = fragPath;
	result.valid = true;

	auto compile_shader_from_file = [&](const String& filePath) -> GLuint {
		String temp = read_file(filePath);
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

			String error(length, 0);
			glGetShaderInfoLog(shaderID, length, &length, &error[0]);
			
			if (length == 0) GL_LOG_ERROR();
			result.valid = false;

			logger.error("[compile_shader_from_file] Shader '{}' compilation errors:\n{}", filePath, &error[0]);
			return 0;
		}

		return shaderID;
	};

	GLuint vertShader = compile_shader_from_file(vertPath);
	defer(glDeleteShader(vertShader));

	GLuint fragShader = compile_shader_from_file(fragPath);
	defer(glDeleteShader(fragShader));

	GLuint program = glCreateProgram();

	glAttachShader(program, vertShader);
	glAttachShader(program, fragShader);

	glLinkProgram(program);

	GLint err = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &err);
	if(err == GL_FALSE) {
		GLint length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

		String error(length, 0);
		glGetProgramInfoLog(program, length, &length, &error[0]);
		
		if (length == 0) GL_LOG_ERROR();

		glDeleteProgram(program);
		result.valid = false;

		logger.error("[compile_shader_program_from_files] ShaderProgram linking errors:\n{}", &error[0]);
	}

	glValidateProgram(program);

	glGetProgramiv(program, GL_VALIDATE_STATUS, &err);
	if(err == GL_FALSE) {
		GLint length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

		std::vector<GLchar> error(length);
		glGetProgramInfoLog(program, length, &length, &error[0]);
		
		if (length == 0) GL_LOG_ERROR();

		glDeleteProgram(program);
		result.valid = false;

		logger.error("[compile_shader_program_from_files] ShaderProgram validation errors:\n{}", &error[0]);
	}

	glDetachShader(program, vertShader);
	glDetachShader(program, fragShader);

	result.programID = program;
	
	// logger.good("[compile_shader_program_from_files] ShaderProgram compiled successful");
	return result;
}

void use_program(const ShaderProgram& shader) {
	if (!shader.valid) {
		logger.fatal("[use_program] Shader program ['{}', '{}'] is invalid", shader.vertPath, shader.fragPath);
	}
	glUseProgram(shader.programID);
}