#include "triangle.h"

Triangle::Triangle() {}

Triangle::Triangle(vec3 bottom_left, vec3 bottom_right, vec3 top, vec3 base_color)
{
	ShapeFactory factory;
	center.x = (double)(bottom_left.x + top.x + bottom_right.x) / 3;
	center.y = (double)(bottom_left.y + top.y + bottom_right.y) / 3;
	center.z = 0;
	meshes.push_back(factory.CreateTriangle(bottom_left, bottom_right, top, base_color));
}

void Triangle::Draw(GLuint program)
{
	for (auto it = meshes.begin(); it != meshes.end(); ++it)
	{
		it->Draw(program);
	}
}