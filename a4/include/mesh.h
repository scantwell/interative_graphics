#ifndef MESH_H
#define MESH_H

#include "Angel.h"
#include<vector>

struct Vertex
{
	vec4 position;
	vec4 color;
	Vertex(vec4 position, vec4 color) : position(position),
		color(color) {}
};

class Mesh 
{
public:
    std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, int primitive_type);
    void Draw(GLuint program);
private:
    GLuint vao, vbo, ebo;
    void initMesh();
	int type; //primitive type ie GL_TRIANGLES, GL_TRIANGLE_STRIP, ..etc
};
#endif