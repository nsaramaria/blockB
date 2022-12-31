#pragma once
#include <GLES2/gl2.h>
#include "cmath"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Map
{
public:
    GLuint terrainVbo = 0;
    GLuint terrainNorm = 0;
	GLuint terrainEbo = 0;
	int terrainCount = 0;

	void genTerrain(int wRes = 128, int lRes = 128);
	Map();

};