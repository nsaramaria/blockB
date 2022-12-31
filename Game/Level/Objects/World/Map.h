#pragma once
#include <GLES2/gl2.h>
#include "cmath"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../../../tmpl8/CubeModel.h"

class Map : public ObjectModel
{
public:
	//textura ,graduala, layers
	int vertices_x = 200;
	int vertices_z = 200;
	int stride = 8;
    int indices_number = (vertices_x - 1) * (vertices_z - 1) * 6;
	GLushort iindices[237606];

	static const int layers = 6;
	float frequency[layers];
	float amplitude[layers];
	float offset[layers];

	Map();
	Map(MyFiles*);

	bool Update(glm::mat4 view); // we supply an update
	bool Draw(); // and a draw 

};
