#ifndef _BODY
#define _BODY
#include "Utils.h"
#include "CurrentState.h"
#include <iostream>
#include <IL/il.h>
using namespace std;

class Body
{
public:
	GLuint
		ProjectionMatrixUniformLocation[2],
		ViewMatrixUniformLocation[2],
		ModelMatrixUniformLocation[2],
		MVMatrixUniformLocation[2],
		ViewVectorUniformLocation[2];

	GLuint BufferIds[3];
	GLuint ShaderIds[6];
	

	GLuint VertNormBuff,
			TexBuffer;

	GLuint image;

	bool success;

	
	GLuint g_gaussSampler;
	GLuint gaussianTextureUnif[2];
	int g_gaussTexUnit;


	
	Matrix ModelMatrix;
	CurrentState state;

	virtual void Create() = 0;
	virtual void Destroy() = 0;
	virtual void Draw(int, int) = 0;
	virtual void DrawShadow() = 0;
	/*virtual GLuint GetViewMatrixUniformLocation() = 0;
	virtual GLuint GetProjectionMatrixUniformLocation() = 0;
	virtual GLuint GetProgramId() = 0;*/
	virtual Matrix GetModelMatrix();
	virtual void SetModelMatrix(Matrix);
};

#endif