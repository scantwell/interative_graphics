#ifndef MESH_H
#define MESH_H

#include "Angel.h"
#include<vector>

struct Vertex
{
	vec3 position;
	vec3 normal;
	Vertex() :
		position(vec3(0, 0, 0)),
		normal(vec3(0, 0, 0)) {}
	Vertex(vec3 position, vec3 normal) : 
		position(position),
		normal(normal) {}
};

struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	Material() {};
	Material(vec4 ambient, vec4 diffuse, vec4 specular, float shininess) :
		ambient(ambient),
		diffuse(diffuse),
		specular(specular),
		shininess(shininess) {}
};

class Mesh 
{
public:
    std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	Material material;
	Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, int primitive_type);
    void Draw(GLuint program);
private:
    GLuint vao, vbo, ebo;
    void initMesh();
	int type; //primitive type ie GL_TRIANGLES, GL_TRIANGLE_STRIP, ..etc
};
#endif