#include "Map.h"

Map::Map()
{
	GLfloat TheVertices[vertices_z * vertices_x * stride];
	int ignored = 0;

    //setting the values manually so I have more control of them
    frequency[0] = 0.07f;
	amplitude[0] = 14.0f;
	offset[0] = 7.0f;
	frequency[1] = 1.0f;
	amplitude[1] = 0.3f;
	frequency[2] = 0.2f;
	amplitude[2] = 5.0f;
	offset[2] = 15.0f;
	frequency[3] = 0.6f;
	amplitude[3] = 0.6f;
	frequency[4] = 1.2f;
	amplitude[4] = 0.3f;

	for (int i = 0; i < vertices_x * vertices_z; i++)
	{
		//Create vertex buffer
		//position
		TheVertices[i * stride] = (i % vertices_x) * 2;
		TheVertices[i * stride + 1] = amplitude[0] * sin(frequency[0] * (i % vertices_x) + offset[0]) + amplitude[0] * sin(frequency[0] * int(i / vertices_x) + offset[0]) +
			amplitude[1] * sin(frequency[1] * (i % vertices_x)) + amplitude[1] * sin(frequency[1] * int(i / vertices_x)) +
			amplitude[2] * sin(frequency[2] * (i % vertices_x) + offset[2]) + amplitude[2] * sin(frequency[2] * int(i / vertices_x) + offset[2]) +
			amplitude[3] * sin(frequency[3] * (i % vertices_x)) + amplitude[3] * sin(frequency[3] * int(i / vertices_x)) +
			amplitude[4] * sin(frequency[4] * (i % vertices_x)) + amplitude[4] * sin(frequency[4] * int(i / vertices_x));
        TheVertices[i * stride + 2] = int(i / vertices_x) * 2;

		//colour Coords
		if (TheVertices[i * stride + 1] > 2.f)
		{
			TheVertices[i * stride + 3] = 1.0f;
			TheVertices[i * stride + 4] = 1.0f;
			TheVertices[i * stride + 5] = 1.0f;
		}
		if (TheVertices[i * stride + 1] > -5.f && TheVertices[i * stride + 1]<= 2.f )
		{
			TheVertices[i * stride + 3] = 0.18f;
			TheVertices[i * stride + 4] = 0.1f;
			TheVertices[i * stride + 5] = 0.0f;
		}
		if (TheVertices[i * stride + 1] <= -2.f)
		{
			TheVertices[i * stride + 3] = 0.0f; //red
			TheVertices[i * stride + 4] = 0.5f; //green
			TheVertices[i * stride + 5] = 0.0f; //blue
		}

		//texture Coords
		if (TheVertices[i * stride + 1] > 2.f)
		{
			if ((i % vertices_x) % 2 == 0)
				TheVertices[i * stride + 6] = 0;
			else TheVertices[i * stride + 6] = 0.33;
		}
		if (TheVertices[i * stride + 1] > -5.f && TheVertices[i * stride + 1] <= 2.f)
		{
			if ((i % vertices_x) % 2 == 0)
				TheVertices[i * stride + 6] = 0.33;
			else TheVertices[i * stride + 6] = 0.66;
		}
		if (TheVertices[i * stride + 1] <= -2.f)
		{
			if ((i % vertices_x) % 2 == 0)
				TheVertices[i * stride + 6] = 0.66;
			else TheVertices[i * stride + 6] = 1;
		}
		 
		if (int(i / vertices_x) % 2 == 0)
			TheVertices[i * stride + 7] = 0.0f;
		else TheVertices[i * stride + 7] = 1.0f;
		//Create index buffer
		if (i % vertices_x == vertices_x - 1)
		{
			ignored++;
			continue;
		}

		if (i + vertices_x >= vertices_x * vertices_z)continue;

		iindices[(i - ignored) * 6] = i;
		iindices[(i - ignored) * 6 + 1] = i + 1;
		iindices[(i - ignored) * 6 + 2] = i + vertices_x + 1;
		iindices[(i - ignored) * 6 + 3] = i;
		iindices[(i - ignored) * 6 + 4] = i + vertices_x + 1;
		iindices[(i - ignored) * 6 + 5] = i + vertices_x;
	}

	mTranslationMatrix = glm::mat4(1.0f);
	//mRotationMatrix = glm::mat4(1.0f);
	//mScaleMatrix = glm::mat4(1.0f);
	Model = glm::mat4(1.0f);  // give the model I as an initialisation
	Scales = glm::vec3(0.0f, 0.0f, 0.0f);
    Vertices = TheVertices;
	SetModelMatrix();

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TheVertices), TheVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Map::Map(MyFiles* FH)
	: Map()
{
	// Load, create texture and generate mipmaps
	int width, height;

	char* image = FH->Load((char*)"../../../Resources/Textures/3textures.jpg", &width, &height);

	glGenTextures(1, &texture1);  // create our 1st texture and store the handle in texture
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, texture1);  // bind it, so it now is what we are looking at
	if (glGetError() != GL_NO_ERROR) printf("Oh bugger, Model graphic init failed\n");
	// set up some paramaters
    // Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// now put the image in there...might be tricky
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);  // Unbind texture when done, so we won't accidentily mess up our texture.

}

bool Map::Update(glm::mat4 view)
{
	mScaleMatrix = glm::mat4(1.0f);
	Scales = glm::vec3(0.1f, 0.1f, 0.1f);

	mScaleMatrix = glm::scale(mScaleMatrix, Scales);
	MakeRotationMatrix();
	MakeTranslationMatrix();
	MakeModelMatrix();

	// get these from the camera normally

   // Projection matrix : 45° Field of View, 4:3 ratio, display range : near 0.1 unit <->  far 100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 200.0f);
	glm::mat4 View = glm::lookAt(glm::vec3(10, 10, 24),/*Camera is at(17, 23, 3), in World Space*/glm::vec3(0, 0, 0),/*look at the origin*/glm::vec3(0, 1, 0) /*pointing up( 0,-1,0 will be upside-down)*/);
		
	glUseProgram(programObject);

	glEnable(GL_DEPTH_TEST);
	// make the MVP 
	glm::mat4 MVP = Projection * view * Model;  // Remember order seems backwards
	GLuint MatrixID = glGetUniformLocation(programObject, "MVP");  //LOOK!!!!
	GLuint AmbID = glGetUniformLocation(programObject, "Ambient");

	float Amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glUniform4fv(AmbID, 1, &Amb[0]);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	return true;
}
bool Map::Draw()
{
	int var;
	var = glGetError();

	if (var != GL_NO_ERROR)
	{
		printf("Oh bugger Map Draw error\n");
		return false;
	}

	// Use the program object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glUseProgram(programObject);

	//load the vertex data
	glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat),0);
	glVertexAttribPointer(colourLoc, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLfloat*)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (GLfloat*)(6 * sizeof(GLfloat)));

	glEnableVertexAttribArray(positionLoc);
	glEnableVertexAttribArray(colourLoc);
	glEnableVertexAttribArray(texCoordLoc);

	glBindTexture(GL_TEXTURE_2D, texture1);
	glUniform1i(samplerLoc, 0);

	//glDrawArrays(GL_TRIANGLES, 0, 36);

	glDrawElements(GL_TRIANGLES, indices_number, GL_UNSIGNED_SHORT, iindices);
	
	glDisableVertexAttribArray(positionLoc);
	glDisableVertexAttribArray(colourLoc);
	glDisableVertexAttribArray(texCoordLoc);

	return true;
}

