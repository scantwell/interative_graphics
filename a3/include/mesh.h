#ifndef MESH_H
#define MESH_H

#include "Angel.h"
#include<vector>

class Mesh {
    public:
        std::vector<vec3> vertices;
		std::vector<vec3> colors;
		Mesh();
		Mesh(std::vector<vec3> vertices, std::vector<vec3> colors, int primitive_type);
        void Draw(GLuint program);
    private:
        GLuint vao, vbo;
        void initMesh();
		int type; //primitive type
};
#endif