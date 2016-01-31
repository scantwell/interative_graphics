#include "circle.h"

Circle::Circle() {}

Circle::Circle(vec3 center, double radius, vec3 base_color) :
	center(center)
{
	ShapeFactory factory;
	meshes.push_back(factory.CreateCircle(center, radius, base_color));
}

void Circle::Draw(GLuint program)
{
	for (auto it = meshes.begin(); it != meshes.end(); ++it)
	{
		it->Draw(program);
	}
}