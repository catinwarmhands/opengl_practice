#pragma once

#include "libs.cpp"

GLuint load_texture_from_file(string filePath) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, depth;
	void* data = stbi_load(filePath.c_str(), &width, &height, &depth, STBI_rgb);
	if (!data) {
		std::cout << "[load_texture_from_file] cant load file '" << filePath << "'" << endl;
		exit(1);
	}
	GLuint textureID;
	glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(data);
    cout << "[load_texture_from_file] texture '" << filePath << "' loaded OK" << endl;
    return textureID;
}