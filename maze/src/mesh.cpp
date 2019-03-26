#pragma once

#include "libs.cpp"

struct Mesh {
	vector<GLfloat> vertexPositions;
	vector<GLfloat> textureCoords;
	vector<GLuint>  indices;
};

void destroy(Mesh& mesh) {
	mesh.vertexPositions.clear();
	mesh.textureCoords.clear();
	mesh.indices.clear();
}