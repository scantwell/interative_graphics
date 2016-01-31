#include <iostream>
#include <vector>
#include "shape_factory.h"
#include "Imodel.h"

class Triangle : public IModel
{
public:
	Triangle();
	Triangle(vec3 bottom_left, vec3 bottom_right, vec3 top, vec3 base_colors);
	void Draw(GLuint program);
	vec3 center;
	std::vector<Mesh> meshes;
};