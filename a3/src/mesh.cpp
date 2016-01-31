#include "mesh.h"

Mesh::Mesh(){}

Mesh::Mesh(std::vector<vec3> vertices, std::vector<vec3> colors, int primitive_type) : 
	vertices(vertices), 
	colors(colors),
	type(primitive_type)
{
    this->initMesh();
}

void Mesh::initMesh()
{
	// Create a vertex array object
	glGenVertexArrays(1, &this->vao);

	// Create and initialize a buffer object
	glGenBuffers(1, &this->vbo);
	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	int v_size = vertices.size() * sizeof(vec3);
	int c_size = colors.size() * sizeof(vec3);

	glBufferData(GL_ARRAY_BUFFER, v_size + c_size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, v_size, &vertices.front());
	glBufferSubData(GL_ARRAY_BUFFER, v_size, c_size, &colors.front());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), BUFFER_OFFSET(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_size));

	glBindVertexArray(0);
}

void Mesh::Draw(GLuint program)
{
	glBindVertexArray(this->vao);
	glDrawArrays(this->type, 0, this->vertices.size());
	glBindVertexArray(0);
}