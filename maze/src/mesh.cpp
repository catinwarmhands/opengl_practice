#pragma once

#include "libs.cpp"
#include "utils.cpp"

struct Mesh {
	vector<GLfloat> vertexPositions;
	vector<GLfloat> textureCoords;
	vector<GLuint>  indices;
	// vector<GLfloat> vertexNormals;
};

// считать меш из .obj файла
// никаких оптимизаций для общих вершин не проводится, не успел просто
Mesh read_mesh(const string& filePath) {
	Mesh mesh;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string err = tinyobj::LoadObj(shapes, materials, filePath.c_str());

	if (!err.empty()) {
		cout << "[read_mesh] Error: '" << err << "'" << endl;
		exit(0);
	}

	int positionsSize = 0;
	for (size_t i = 0; i < shapes.size(); i++) {
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			for (int j = 0; j < 3; ++j) {
				mesh.vertexPositions.push_back(shapes[i].mesh.positions[3*v+j]);
			}
		}
		for (size_t t = 0; t < shapes[i].mesh.texcoords.size() / 2; t++) {
			for (int j = 0; j < 2; ++j) {
				mesh.textureCoords.push_back(shapes[i].mesh.texcoords[2*t+j]);
			}
		}
		for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
			for (int j = 0; j < 3; ++j) {
				mesh.indices.push_back(positionsSize+shapes[i].mesh.indices[3*f+j]);
			}
		}
		positionsSize = mesh.vertexPositions.size()/3;
	}

	return mesh;
}


void destroy(Mesh& mesh) {
	mesh.vertexPositions.clear();
	mesh.textureCoords.clear();
	mesh.indices.clear();
}