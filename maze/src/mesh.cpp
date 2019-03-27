#pragma once

#include "libs.cpp"
#include "utils.cpp"

struct Mesh {
	vector<GLfloat> vertexPositions;
	vector<GLfloat> textureCoords;
	vector<GLuint>  indices;
	vector<GLfloat> vertexNormals;
};


// считать меш из .obj файла
// ВНИМАНИЕ: библииотека objloader неправильно считывает значения, если в файле стоят лишние пробелы
// и вообще крашится на больших объектах
// алсо, никаких оптимизаций для общих вершин не проводится, не успел просто
Mesh read_mesh(const string& filePath) {
	const char *fileData = read_file(filePath).c_str();
	objl_obj_file ObjFile;
	objl_LoadObjMalloc((char*)fileData, &ObjFile);

	Mesh mesh;

	for (int i = 0; i < ObjFile.f_count; ++i) {
		for (int j : {
			ObjFile.f[i].f0.vertex-1,
			ObjFile.f[i].f1.vertex-1,
			ObjFile.f[i].f2.vertex-1,
		}) {
			vec3 pos(ObjFile.v[j].x, ObjFile.v[j].y, ObjFile.v[j].z);
			mesh.vertexPositions.insert(mesh.vertexPositions.end(), {pos.x, pos.y, pos.z});
		}

		for (int j : {
			ObjFile.f[i].f0.texture-1,
			ObjFile.f[i].f1.texture-1,
			ObjFile.f[i].f2.texture-1,
		}) {
			vec2 uv(ObjFile.vt[j].x, ObjFile.vt[j].y);
			mesh.textureCoords.insert(mesh.textureCoords.end(), {uv.x, uv.y});
		}

		for (int j : {
			ObjFile.f[i].f0.normal-1,
			ObjFile.f[i].f1.normal-1,
			ObjFile.f[i].f2.normal-1,
		}) {
			vec3 normal(ObjFile.vn[j].x, ObjFile.vn[j].y, ObjFile.vn[j].z);
			mesh.vertexNormals.insert(mesh.vertexNormals.end(), {normal.x, normal.y, normal.z});
		}
	}
	for (int i = 0; i < mesh.vertexPositions.size(); ++i) {
		mesh.indices.push_back(i);
	}

	objl_FreeObj(&ObjFile);
	return mesh;
}


void destroy(Mesh& mesh) {
	mesh.vertexPositions.clear();
	mesh.textureCoords.clear();
	mesh.indices.clear();
}