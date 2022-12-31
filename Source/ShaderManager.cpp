#include "ShaderManager.h"

#include <iostream> // we're going to be loading files so need these
#include <fstream>
#include <string>

using namespace std;

ShaderManager::ShaderManager()
{
	//empty
};

ShaderManager::~ShaderManager()
{
	//empty (probably should unload the shaders?)
};



//link a V and F shader together
GLuint ShaderManager::MakeProgramObject(GLuint VShad, GLuint FShad)
{
	
	
// Create the program object	
	GLuint PO = glCreateProgram();
	
	if (PO == 0) 	return FALSE;
	
// now we have the V and F shaders  attach them to the progam object
	glAttachShader(PO, VShad);
	glAttachShader(PO, FShad);
	
// Link the program
	glLinkProgram(PO);
// Check the link status
	GLint LinkedOk;
	
	glGetProgramiv(PO, GL_LINK_STATUS, &LinkedOk);
	if (!LinkedOk)
	{
		GLint RetinfoLen = 0; // check and report any errors using standard system

		glGetProgramiv(PO, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1)
		{
			GLchar* infoLog = (GLchar*)malloc(sizeof(char) * RetinfoLen);
			glGetProgramInfoLog(PO, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "Error linking program:\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteProgram(PO);
		return FALSE;
	}
	
	
	return PO;
}

GLuint ShaderManager::LoadAndMakeShader(MyFiles* FH, GLenum type, const char* Fname)
{
	const char* ShaderTxt = FH->LoadText(Fname);

	// print out the shader		
	printf("shader loaded from filename %s \n %s\n", Fname, ShaderTxt);

	
	GLuint Shader = MakeShader(type, ShaderTxt);
	
	if (Shader == 0) printf("Failed to compile Shader/n");

	return Shader;
	
}


/*
 Now we have be able to create a shader object, pass the shader source
 and them compile the shader.
*/
GLuint ShaderManager::MakeShader(GLenum type, const char *shaderSrc)
{	

	GLuint TheShader = glCreateShader(type);	// 1st create the shader object
	
	if (TheShader == 0) return FALSE; // can't allocate so stop.
	

	glShaderSource(TheShader, 1, &shaderSrc, NULL); // pass the shader source then compile it
	glCompileShader(TheShader);
	
	GLint  IsItCompiled;
	

	glGetShaderiv(TheShader, GL_COMPILE_STATUS, &IsItCompiled); // After the compile we need to check the status and report any errors
	if (!IsItCompiled)
	{
		GLint RetinfoLen = 0;
		glGetShaderiv(TheShader, GL_INFO_LOG_LENGTH, &RetinfoLen);
		if (RetinfoLen > 1)
		{ // standard output for errors
			char* infoLog = (char*) malloc(sizeof(char) * RetinfoLen);
		 	glGetShaderInfoLog(TheShader, RetinfoLen, NULL, infoLog);
			fprintf(stderr, "There was an error Error compiling shader:\n %s\n %s\n", shaderSrc, infoLog);
			free(infoLog);
		}
		glDeleteShader(TheShader);
		return FALSE;
	}
	return TheShader;
}

