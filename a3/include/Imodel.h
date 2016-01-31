#ifndef IMODEL_H
#define IMODEL_H

#include <iostream>
#include <vector>


class IModel
{
public:
	virtual void Draw(GLuint program) = 0;
private:
	std::vector<Mesh> meshes;
};
#endif