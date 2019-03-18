#pragma once

#include "libs.cpp"
#include "utils.cpp"

struct Image {
	byte* data;
	int height, width, depth;
};

Image load_image_from_file(string filePath) {
	Image im;
	stbi_set_flip_vertically_on_load(true);
	im.data = stbi_load(filePath.c_str(), &im.width, &im.height, &im.depth, STBI_rgb);

	if (!im.data) {
		std::cout << "[load_image_from_file] cant load file '" << filePath << "'" << endl;
		exit(1);
	}

	return im;
}

void destroy_image(Image image) {
	stbi_image_free(image.data);
}

GLuint load_texture_from_file(string filePath) {
	Image image = load_image_from_file(filePath);
	GLuint textureID;
	glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    destroy_image(image);
    cout << "[load_texture_from_file] texture '" << filePath << "' loaded OK" << endl;
    return textureID;
}