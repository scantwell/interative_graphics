#include <iostream>
#include <vector>
#include "shape_factory.h"
#include "Imodel.h"

class Circle : public IModel
{
public:
	Circle();
	Circle(vec3 center, double radius, vec3 base_color);
	void Draw(GLuint program);
	vec3 center;
	std::vector<Mesh> meshes;
};