#include "shape_factory.h"

const double PI = 3.1415;

Mesh ShapeFactory::CreateCircle(vec3 center, double radius, vec3 base_color)
{
	std::vector<vec3> vertices;
	std::vector<vec3> colors;
	double x, y, angle;

	for (double degrees = 360; degrees >= 0; degrees -= 1)
	{
		angle = (double)(degrees * PI) / 180;
		x = radius * cos(angle);
		y = radius * sin(angle);
		x += center.x;
		y += center.y;

		vertices.push_back(vec3(x, y, 0));
		colors.push_back(base_color);
	}
	vertices.push_back(vertices.at(0));
	colors.push_back(colors.at(0));
	return Mesh(vertices, colors, GL_TRIANGLE_FAN);
}

Mesh ShapeFactory::CreateEllipse(vec3 center, double radius_x, double radius_y, vec3 base_color)
{
	std::vector<vec3> vertices;
	std::vector<vec3> colors;
	double x, y;

	for (float angle = 0; angle < (2.0f * PI); angle += 0.001)
	{
		x = radius_x * cos(angle);
		y = radius_y * sin(angle);
		x += center.x;
		y += center.y;

		vertices.push_back(vec3(x, y, 0));
		colors.push_back(base_color);
	}
	vertices.push_back(vertices.at(0));
	colors.push_back(colors.at(0));
	return Mesh(vertices, colors, GL_TRIANGLE_FAN);
}

Mesh ShapeFactory::CreateRectangle(vec3 min, vec3 max, vec3 base_color)
{
	std::vector<vec3> vertices;
	std::vector<vec3> colors;
	
	vertices.push_back(vec3(min.x, max.y, 0));
	vertices.push_back(min);
	vertices.push_back(max);
	vertices.push_back(vec3(max.x, min.y, 0));
	colors.push_back(base_color);
	colors.push_back(base_color);
	colors.push_back(base_color);
	colors.push_back(base_color);
	return Mesh(vertices, colors, GL_TRIANGLE_STRIP);
}

Mesh ShapeFactory::CreateTriangle(vec3 bottom_left, vec3 bottom_right, vec3 top, vec3 base_color)
{
	std::vector<vec3> vertices;
	std::vector<vec3> colors;

	vertices.push_back(bottom_left);
	vertices.push_back(bottom_right);
	vertices.push_back(top);
	colors.push_back(base_color);
	colors.push_back(base_color);
	colors.push_back(base_color);
	return Mesh(vertices, colors, GL_TRIANGLES);
}