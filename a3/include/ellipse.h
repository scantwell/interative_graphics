#include <iostream>
#include <vector>
#include "shape_factory.h"
#include "Imodel.h"

class Ellipse : public IModel
{
public:
	Ellipse();
	Ellipse(vec3 center, double radius_x, double radius_y, vec3 base_color);
	void Draw(GLuint program);
	vec3 center;
	std::vector<Mesh> meshes;
};