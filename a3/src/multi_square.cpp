#include "multi_square.h"

void
print_vector(std::vector<vec3> v)
{
	for (std::vector<vec3>::const_iterator i = v.begin(); i != v.end(); ++i)
		std::cout << *i << ' ' << std::endl;
	std::cout << "SIZE: " << v.size() << std::endl;
}

MultiSquare::MultiSquare(vec3 base_color)
{
	ShapeFactory factory;
	vec3 color, black(0, 0, 0);
	vec3 bottom_left = vec3(-0.35, -0.35, 0);
	vec3 top_right = vec3(0.35, 0.35, 0);
	double factor = 0.05;

	for (int i = 0; i < 6; ++i)
	{
		color = base_color;
		if (i % 2 == 1)
		{
			color = black;
		}
		meshes.push_back(factory.CreateRectangle(bottom_left, top_right, color));
		bottom_left = vec3(bottom_left.x + factor, bottom_left.y + factor, 0);
		top_right = vec3(top_right.x - factor, top_right.y - factor, 0);
	}
}

void MultiSquare::Draw(GLuint program)
{
	for (auto it = meshes.begin(); it != meshes.end(); ++it)
	{
		it->Draw(program);
	}
}