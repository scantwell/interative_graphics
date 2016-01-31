#include "ellipse.h"

Ellipse::Ellipse() {}

Ellipse::Ellipse(vec3 center, double radius_x, double radius_y, vec3 base_color) :
	center(center)
{
	ShapeFactory factory;
	meshes.push_back(factory.CreateEllipse(center, radius_x, radius_y, base_color));
}

void Ellipse::Draw(GLuint program)
{
	for (auto it = meshes.begin(); it != meshes.end(); ++it)
	{
		it->Draw(program);
	}
}