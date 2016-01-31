#include <iostream>
#include <vector>


class IModel
{
public:
	virtural void Draw(GLuint program) = 0;
private:
	std::vector<Mesh> meshes;
};