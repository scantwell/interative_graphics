#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <vector>
#include "Angel.h"
#include "mesh.h"


class ShapeFactory
{
public:
	static Mesh CreateCircle(vec3 center, double radius, vec3 base_color);
	static Mesh CreateEllipse(vec3 center, double radius_x, double radius_y, vec3 base_color);
	static Mesh CreateRectangle(vec3 min, vec3 max, vec3 base_color);
	static Mesh CreateTriangle(vec3 bottom_left, vec3 bottom_right, vec3 top, vec3 base_color);
};

#endif