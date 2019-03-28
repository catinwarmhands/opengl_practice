#pragma once

#include "libs.cpp"
#include "mesh.cpp"

struct Model {
	GLuint VAO, EBO;
	GLsizei indicesCount;
	vector<GLuint> buffers;
	Mesh* mesh;
};

GLuint make_VBO(const vector<GLfloat>& bufferData, GLuint index, GLuint dimensions) {
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferData.size()*sizeof(GLfloat), bufferData.data(), GL_STATIC_DRAW);

	// TODO bind VAO
	glVertexAttribPointer(index, dimensions, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(index);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return VBO;
}

GLuint make_EBO(const vector<GLuint>& indices) {
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return EBO;
}

Model make_model(Mesh *mesh) {
	Model model;
	model.mesh = mesh;
	model.indicesCount = mesh->indices.size();

	glGenVertexArrays(1, &model.VAO);
	glBindVertexArray(model.VAO);

	GLuint vertexPositionsBuffer = make_VBO(mesh->vertexPositions, 0, 3);
	GLuint textureCoordsBuffer   = make_VBO(mesh->textureCoords,   1, 2);

	model.buffers = {
		vertexPositionsBuffer,
		textureCoordsBuffer
	};

	model.EBO = make_EBO(mesh->indices);

	glBindVertexArray(0);
	return model;
}

void destroy(Model& model) {
	glDeleteVertexArrays(1, &model.VAO);
	glDeleteBuffers(model.buffers.size(), model.buffers.data());
	glDeleteBuffers(model.buffers.size(), &model.EBO);
	model.buffers.clear();
	model.VAO = model.EBO = model.indicesCount= 0;
}
