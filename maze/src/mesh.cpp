#pragma once

#include "libs.cpp"
#include "utils.cpp"

struct Mesh {
	vector<GLfloat> vertexPositions;
	vector<GLfloat> textureCoords;
	vector<GLuint>  indices;
	// vector<GLfloat> vertexNormals;
};

static void PrintInfo(const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials)
{
  std::cout << "# of shapes    : " << shapes.size() << std::endl;
  std::cout << "# of materials : " << materials.size() << std::endl;

  for (size_t i = 0; i < shapes.size(); i++) {
    printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
    printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size()/3);
    printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
    assert((shapes[i].mesh.indices.size() % 3) == 0);
    for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
      printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3*f+0], shapes[i].mesh.indices[3*f+1], shapes[i].mesh.indices[3*f+2], shapes[i].mesh.material_ids[f]);
    }

    printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size()/3);
    assert((shapes[i].mesh.positions.size() % 3) == 0);
    for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
      printf("  v[%ld] = (%f, %f, %f)\n", v,
        shapes[i].mesh.positions[3*v+0],
        shapes[i].mesh.positions[3*v+1],
        shapes[i].mesh.positions[3*v+2]);
    }
  }

}


// считать меш из .obj файла
// ВНИМАНИЕ: библииотека objloader неправильно считывает значения, если в файле стоят лишние пробелы
// и вообще крашится на больших объектах
// алсо, никаких оптимизаций для общих вершин не проводится, не успел просто
Mesh read_mesh(const string& filePath) {
	// 	vector<tinyobj::shape_t> shapes;
	// vector<tinyobj::material_t> materials;
	// string warn, err;
	// tinyobj::attrib_t attrib;
	// bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str());
	Mesh mesh;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string err = tinyobj::LoadObj(shapes, materials, filePath.c_str());

	if (!err.empty()) {
		cout << "[read_mesh] Error: '" << err << "'" << endl;
		exit(0);
	}

	PrintInfo(shapes, materials);
	// exit(0);
	
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
			for (int j = 0; j < 2; ++j) {
				mesh.indices.push_back(shapes[i].mesh.indices[3*f+j]);
			}
		}
	}

	return mesh;
}


void destroy(Mesh& mesh) {
	mesh.vertexPositions.clear();
	mesh.textureCoords.clear();
	mesh.indices.clear();
}