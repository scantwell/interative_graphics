#include <iostream>
#include <vector>
#include "shape_factory.h"
#include "Imodel.h"


class MultiSquare : public IModel
{
public:
	MultiSquare(vec3 base_color);
	void Draw(GLuint program);
private:
	std::vector<Mesh> meshes;
};