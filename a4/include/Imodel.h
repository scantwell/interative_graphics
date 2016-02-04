#include <iostream>
#include <vector>

class Model
{
public:
	virtual void Draw(GLuint program) = 0;
private:
	std::vector<Mesh> meshes;
};