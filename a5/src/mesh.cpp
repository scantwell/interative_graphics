#include "mesh.h"

Mesh::Mesh(){}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, Material material, int primitive_type) : 
	vertices(vertices), 
	indices(indices),
	material(material),
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

	// Enable normal attribute in vertex shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));

	// Bind the default vao
	glBindVertexArray(0);
}

void Mesh::Draw(GLuint program)
{
	glBindVertexArray(this->vao);
	GLint ambientLoc = glGetUniformLocation(program, "material.ambient");
	GLint diffuseLoc = glGetUniformLocation(program, "material.diffuse");
	GLint specularLoc = glGetUniformLocation(program, "material.specular");
	glUniform3f(ambientLoc, this->material.ambient.x, this->material.ambient.y, this->material.ambient.z);
	glUniform3f(diffuseLoc, this->material.diffuse.x, this->material.diffuse.y, this->material.diffuse.z);
	glUniform3f(specularLoc, this->material.specular.x, this->material.specular.y, this->material.specular.z);
	glDrawElements(this->type, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}