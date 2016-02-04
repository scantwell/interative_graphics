#include <model.h>

// Creates a quad by two triangles
// taken from lecture 8
void addQuad(std::vector<GLuint> & indices, GLuint a, GLuint b, GLuint c, GLuint d)
{
	indices.push_back(a);
	indices.push_back(b);
	indices.push_back(c);
	indices.push_back(a);
	indices.push_back(c);
	indices.push_back(d);
}

Model::Model() 
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	// Create cube vertices
	// taken from lecture 8
	vertices.push_back(Vertex(vec4(-0.5, -0.5, 0.5, 1.0),  vec4(0, 0, 0, 1)));
	vertices.push_back(Vertex(vec4(-0.5, 0.5, 0.5, 1.0),   vec4(1, 0, 0, 1)));
	vertices.push_back(Vertex(vec4(0.5, 0.5, 0.5, 1.0),    vec4(1, 1, 0, 1)));
	vertices.push_back(Vertex(vec4(0.5, -0.5, 0.5, 1.0),   vec4(0, 1, 0, 1)));
	vertices.push_back(Vertex(vec4(-0.5, -0.5, -0.5, 1.0), vec4(0, 0, 1, 1)));
	vertices.push_back(Vertex(vec4(-0.5, 0.5, -0.5, 1.0),  vec4(1, 0, 1, 1)));
	vertices.push_back(Vertex(vec4(0.5, 0.5, -0.5, 1.0),   vec4(1, 1, 1, 1)));
	vertices.push_back(Vertex(vec4(0.5, -0.5, -0.5, 1.0),  vec4(0, 1, 1, 1)));

	// Create the quads for each face, taken from lecture 8
	addQuad(indices, 1, 0, 3, 2);
	addQuad(indices, 2, 3, 7, 6);
	addQuad(indices, 3, 0, 4, 7);
	addQuad(indices, 6, 5, 1, 2);
	addQuad(indices, 4, 5, 6, 7);
	addQuad(indices, 5, 4, 0, 1);

	meshes.push_back(Mesh(vertices, indices, GL_TRIANGLES));
}

void Model::Draw(GLuint program)
{
	for (auto it = meshes.begin(); it != meshes.end(); ++it)
	{
		it->Draw(program);
	}
}
	