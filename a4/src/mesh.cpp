#include "mesh.h"

Mesh::Mesh(){}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, int primitive_type) : 
	vertices(vertices), 
	indices(indices),
	type(primitive_type)
{
    this->initMesh();
}

void Mesh::initMesh()
{
	// Create a vertex array object
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	// Create and initialize a buffer object
	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	int v_size = vertices.size() * sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER, v_size, &vertices.front(), GL_STATIC_DRAW);

	// Create and initialize a elements buffer object
	glGenBuffers(1, &this->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	int e_size = indices.size() * sizeof(GLuint);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, e_size, &indices.front(), GL_STATIC_DRAW);

	// Enable position attribute in vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));

	// Enable color attribute in vertex shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));

	// Bind the default vao
	glBindVertexArray(0);
}

void Mesh::Draw(GLuint program)
{
	glBindVertexArray(this->vao);
	glDrawElements(this->type, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}