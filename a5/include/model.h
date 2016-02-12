#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <string>
#include <mesh.h>

class Model
{
public:
	Model(const char* path);
	void Draw(GLuint program);
private:
	std::vector<Mesh> meshes;
	void load(const char* path);
};