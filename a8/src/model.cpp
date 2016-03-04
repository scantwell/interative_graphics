#include <model.h>

Model::Model(const char* path)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	this->load(path);
}

void Model::load(const char* path)
{
	std::ifstream fin(path);
	std::string line;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	if (!fin)
	{
		std::cout << "Error loading model from '" << path << "'." << std::endl;
	}
	while (std::getline(fin, line))
	{
		if (line.length() <= 1)
			continue;

		std::istringstream iss(line);
		std::vector<std::string> tokens{ 
			std::istream_iterator<std::string>{iss}, 
			std::istream_iterator<std::string>{} };

		if (tokens.front() == "v")
		{  
			vertices.push_back( Vertex( vec3((double)std::stof(tokens[1]), (double)std::stof(tokens[2]), (double)std::stof(tokens[3])), vec3(0, 0, 0)));
		}
		else if (tokens.front() == "f")
		{
			GLuint idx1 = (GLuint)std::stoi(tokens[1]) - 1;
			GLuint idx2 = (GLuint)std::stoi(tokens[2]) - 1;
			GLuint idx3 = (GLuint)std::stoi(tokens[3]) - 1;

			indices.push_back(idx1);
			indices.push_back(idx2);
			indices.push_back(idx3);

			vec3 u = vertices[idx2].position - vertices[idx1].position;
			vec3 v = vertices[idx3].position - vertices[idx1].position;
			vec3 normal = normalize(cross(u, v));

			vertices[idx1].normal = normalize(vertices[idx1].normal + normal);
			vertices[idx2].normal = normalize(vertices[idx2].normal + normal);
			vertices[idx2].normal = normalize(vertices[idx2].normal + normal);
		}
		else
		{
			continue;
		}
	}
	this->meshes.push_back(Mesh(vertices, indices, GL_TRIANGLES));
}

void Model::Draw(GLuint program)
{
	for (auto it = meshes.begin(); it != meshes.end(); ++it)
	{
		it->Draw(program);
	}
}

void Model::SetMaterial(Material material)
{
	for (auto it = meshes.begin(); it != meshes.end(); ++it)
	{
		it->material = material;
	}
}
	