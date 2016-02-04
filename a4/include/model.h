#include <iostream>
#include <vector>
#include <mesh.h>

class Model
{
public:
	Model();
	void Draw(GLuint program);
private:
	std::vector<Mesh> meshes;
};