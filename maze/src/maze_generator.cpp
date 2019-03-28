#pragma once

#include "libs.cpp"
#include "mesh.cpp"
#include "model.cpp"
// #include "globals.cpp"

void draw_maze_matrix(int* mazeMatrix, int n, int m) {
	const float size = 2.0f/n;
	vec2 origin = {-1, 1};
	vec2 cur;
	for (int i = 0; i < n; ++i) {
		cur.y = origin.y - size*i;
		for (int j = 0; j < m; ++j) {
			cur.x = origin.x + size*j;
			int c = mazeMatrix[i*m+j];
			switch (c) {
				case -1: glColor3f(1, 0, 0); break;
				case 0:  glColor3f(0, 1, 0); break;
				case 1:  glColor3f(0, 0, 0); break;
			}
			glBegin(GL_QUADS);
				glVertex2f(cur.x,      cur.y-size);
				glVertex2f(cur.x+size, cur.y-size);
				glVertex2f(cur.x+size, cur.y);
				glVertex2f(cur.x,      cur.y);
			glEnd();
		}
	}
}

bool is_in_interval(ivec2 v, int x1, int x2, int y1, int y2) {
	return x1 <= v.x && v.x <= x2 && y1 <= v.y && v.y <= y2;
}

// сгенерировать матрицу лабиринта размера n строк на m столбцов
// n и m доблжны быть нечетными
// алгоритм: https://habr.com/ru/post/262345/
enum MazeCellType {
	UNVISITED = -1,
	EMPTY     =  0,
	WALL      =  1,
	COIN      =  2,
	PLAYER    =  3
};

int* generate_maze_matrix(int n, int m) {
	int* maze = new int[n*m];

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			int pos = i*m+j;
			if (i % 2 == 0 || j % 2 == 0)
				maze[pos] = WALL;
			else
				maze[pos] = UNVISITED;
		}
	}

	stack<ivec2> history;

	ivec2 cur = {linearRand(1, m-2), linearRand(1, n-2)};
	cur.x = cur.x + (1-cur.x%2);
	cur.y = cur.y + (1-cur.y%2);

	while (true) {
		int unvisidedIndex = -1;
		for (int i = 0; i < n*m; ++i)
			if (maze[i] == UNVISITED)
				unvisidedIndex = i;

		if (unvisidedIndex == -1)
			break;

		vector<ivec2> neighbours;
		for (int i = 0; i < 4; ++i) {
			ivec2 v = cur;
			switch (i) {
				case 0: v += ivec2(-2, 0); break;
				case 1: v += ivec2( 2, 0); break;
				case 2: v += ivec2( 0,-2); break;
				case 3: v += ivec2( 0, 2); break;
			}
			if (is_in_interval(v, 1, m-2, 1, n-2) && maze[v.y*m + v.x] == UNVISITED)
				neighbours.push_back(v);
		}

		// debug draw
		#if 0
			glfwPollEvents();
			if (glfwWindowShouldClose(window))
				exit(0);
			glClear(GL_COLOR_BUFFER_BIT);
			draw_maze_matrix(maze, n, m);
			glfwSwapBuffers(window);
			// Sleep(10);
		#endif

		if (neighbours.size() != 0) {
			history.push(cur);
			ivec2 target = neighbours[linearRand(0, (int)neighbours.size()-1)];
			ivec2 wall = cur + ivec2(-sgn(cur.x-target.x), -sgn(cur.y-target.y));
			maze[cur.y*m+cur.x] = maze[wall.y*m+wall.x] = maze[target.y*m+target.x] = EMPTY;
			cur = target;
		} else if (history.size() != 0) {
			cur = history.top();
			history.pop();
		} else {
			cur = {unvisidedIndex%n, unvisidedIndex/n};
		}
	}

	int doorsCount = min(n, m)/2;
	int failsCount = 0;
	int maxFailsCount = 100;
	for (int i = 0; i < doorsCount; ++i) {
		cur = {linearRand(1, m-2), linearRand(1, n-2)};
		if (maze[cur.y*m + cur.x] != WALL) {
			--i;
			continue;
		}
		ivec2 up        = cur + ivec2( 0, -1);
		ivec2 down      = cur + ivec2( 0,  1);
		ivec2 left      = cur + ivec2(-1,  0);
		ivec2 right     = cur + ivec2( 1,  0);
		ivec2 leftup    = cur + ivec2(-1, -1);
		ivec2 rightup   = cur + ivec2( 1, -1);
		ivec2 leftdown  = cur + ivec2(-1,  1);
		ivec2 rigthdown = cur + ivec2( 1,  1);
		if ((!is_in_interval(up, 1, m-2, 1, n-2)
			|| !is_in_interval(down, 1, m-2, 1, n-2)
			|| !is_in_interval(left, 1, m-2, 1, n-2)
			|| !is_in_interval(right, 1, m-2, 1, n-2))
			|| maze[leftup.y*m + leftup.x] != EMPTY
			|| maze[rightup.y*m + rightup.x] != EMPTY
			|| maze[leftdown.y*m + leftdown.x] != EMPTY
			|| maze[rigthdown.y*m + rigthdown.x] != EMPTY
			|| !(maze[up.y*m + up.x] == maze[down.y*m + down.x] && maze[left.y*m + left.x] == maze[right.y*m + right.x] && maze[up.y*m + up.x] != maze[right.y*m + right.x])
		) {
			++failsCount;
			if (failsCount >= maxFailsCount)
				break;
			--i;
			continue;
		}
		maze[cur.y*m + cur.x] = EMPTY;
		failsCount = 0;
	}

	int coinsCount = max(n, m);
	for (int i = 0; i < coinsCount; ++i) {
		cur = {linearRand(1, m-2), linearRand(1, n-2)};
		if (maze[cur.y*m + cur.x] != EMPTY) {
			--i;
			continue;
		}
		maze[cur.y*m + cur.x] = COIN;
	}

	while (true) {
		cur = {linearRand(1, m-2), linearRand(1, n-2)};
		if (maze[cur.y*m + cur.x] == EMPTY) {
			maze[cur.y*m + cur.x] = PLAYER;
			break;
		}
	}

	return maze;
}


Mesh make_mesh_from_maze_matrix(int* mazeMatrix, int n, int m) {
	Mesh mesh;
	vec3 size = {1, 1, 1};
	vec2 origin = {0, 0};
	vec2 cur;
	const float oneThird  = 1.0f/3.0f;
	const float twoThirds = 2.0f/3.0f;
	int k = 0;
	for (int i = 0; i < n; ++i) {
		cur.y = origin.y - size.z*i;
		for (int j = 0; j < m; ++j) {
			cur.x = origin.x + size.x*j;
			int c = mazeMatrix[i*m+j];
			if (c == WALL) {
				vec2 offset = {cur.x*size.x, cur.y*size.z};
				mesh.vertexPositions.insert(mesh.vertexPositions.end(), {
					-0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x,  0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x,  0.5f*size.y, -0.5f+offset.y,
					-0.5f+offset.x,  0.5f*size.y, -0.5f+offset.y,
					-0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,

					-0.5f+offset.x, -0.5f*size.y,  0.5f+offset.y,
					 0.5f+offset.x, -0.5f*size.y,  0.5f+offset.y,
					 0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,
					 0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,
					-0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,
					-0.5f+offset.x, -0.5f*size.y,  0.5f+offset.y,

					-0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,
					-0.5f+offset.x,  0.5f*size.y, -0.5f+offset.y,
					-0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,
					-0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,
					-0.5f+offset.x, -0.5f*size.y,  0.5f+offset.y,
					-0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,

					 0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,
					 0.5f+offset.x,  0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x, -0.5f*size.y,  0.5f+offset.y,
					 0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,
					
					-0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x, -0.5f*size.y,  0.5f+offset.y,
					 0.5f+offset.x, -0.5f*size.y,  0.5f+offset.y,
					-0.5f+offset.x, -0.5f*size.y,  0.5f+offset.y,
					-0.5f+offset.x, -0.5f*size.y, -0.5f+offset.y,
					
					-0.5f+offset.x,  0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x,  0.5f*size.y, -0.5f+offset.y,
					 0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,
					 0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,
					-0.5f+offset.x,  0.5f*size.y,  0.5f+offset.y,
					-0.5f+offset.x,  0.5f*size.y, -0.5f+offset.y
				});
				// GLuint indicesCount = mesh.indices.size();
				// mesh.indices.insert(mesh.indices.end(), {
				// 	0,1,2,
				// 	0,2,3,
				// 	1,5,6,
				// 	1,6,2,
				// 	5,4,7,
				// 	5,7,6,
				// 	4,0,3,
				// 	4,3,7,
				// 	1,0,4,
				// 	1,4,5,
				// 	3,2,6,
				// 	3,6,7
				// });
				
				mesh.textureCoords.insert(mesh.textureCoords.end(), {
					0.0f, oneThird,
					1.0f, oneThird,
					1.0f, twoThirds,
					1.0f, twoThirds,
					0.0f, twoThirds,
					0.0f, oneThird,

					0.0f, oneThird,
					1.0f, oneThird,
					1.0f, twoThirds,
					1.0f, twoThirds,
					0.0f, twoThirds,
					0.0f, oneThird,

					0.0f, twoThirds,
					1.0f, twoThirds,
					1.0f, oneThird,
					1.0f, oneThird,
					0.0f, oneThird,
					0.0f, twoThirds,

					0.0f, twoThirds,
					1.0f, twoThirds,
					1.0f, oneThird,
					1.0f, oneThird,
					0.0f, oneThird,
					0.0f, twoThirds,

					0.0f, oneThird,
					1.0f, oneThird,
					1.0f, 0.0f,
					1.0f, 0.0f,
					0.0f, 0.0f,
					0.0f, oneThird,

					0.0f, 1.0f,
					1.0f, 1.0f,
					1.0f, twoThirds,
					1.0f, twoThirds,
					0.0f, twoThirds,
					0.0f, 1.0f
				});
				// for (int p = indicesCount; p < mesh.indices.size(); ++p) {
				// 	mesh.indices[p] += k*8;
				// }
				k++;
			}
		}
	}
	for (int i = 0; i < mesh.vertexPositions.size(); ++i) {
		mesh.indices.push_back(i);
	}
	return mesh;
}

Mesh generate_ground_mesh(int n, int m) {
	Mesh mesh;
	mesh.vertexPositions = {
		 -0.5f, -0.5f,   +0.5f,
		m-0.5f, -0.5f,   +0.5f,
		m-0.5f, -0.5f, -n+0.5f,
		 -0.5f, -0.5f, -n+0.5f,
	};

	mesh.indices = {
		3,2,1,
		3,1,0
	};

	mesh.textureCoords = {
		0.0f,     0.0f,
		0.0f,     (float)m,
		(float)n, (float)m,
		(float)n, 0.0f
	};
	return mesh;
}

vector<vec3> get_coins_positions_from_maze_matrix(int* mazeMatrix, int n, int m) {
	vector<vec3> result;
	vec3 size = {1, 1, 1};
	vec2 origin = {0, 0};
	vec2 cur;
	for (int i = 0; i < n; ++i) {
		cur.y = origin.y - size.z*i;
		for (int j = 0; j < m; ++j) {
			cur.x = origin.x + size.x*j;
			int c = mazeMatrix[i*m+j];
			if (c == COIN) {
				result.push_back({cur.x, 0, cur.y});
			}
		}
	}
	return result;
}

vec3 get_player_position_from_maze_matrix(int* mazeMatrix, int n, int m) {
	int nm = n*m;
	for (int i = 0; i < nm; ++i) {
		if (mazeMatrix[i] == PLAYER) {
			int x = i % m;
			int z = i / m;
			return {x, 0, -z};
		}
	}
	return {-1, 0, -1};
}

vector<vec3> generate_cubes_positions(int n, int m) {
	int cubeCount = min(n, m);
	vector<vec3> cubesPositions;
	for (int i = 0; i < cubeCount; ++i) {
		cubesPositions.push_back(vec3(
			linearRand(0, m),
			linearRand(3, 6),
			linearRand(-n, 0)
		));
	}
	return cubesPositions;
}