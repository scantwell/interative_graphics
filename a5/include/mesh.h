#ifndef MESH_H
#define MESH_H

#include "Angel.h"
#include<vector>

struct Vertex
{
	vec3 position;
	vec3 normal;
	Vertex(vec3 position, vec3 normal) : 
		position(position),
		normal(normal) {}
};

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	Material() {};
	Material(vec3 ambient, vec3 diffuse, vec3 specular) :
		ambient(ambient),
		diffuse(diffuse),
		specular(specular) {}
};

class Mesh 
{
public:
    std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	Material material;
	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Material material, int primitive_type);
    void Draw(GLuint program);
private:
    GLuint vao, vbo, ebo;
    void initMesh();
	int type; //primitive type ie GL_TRIANGLES, GL_TRIANGLE_STRIP, ..etc
};
#endif