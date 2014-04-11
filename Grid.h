#ifndef _GRID
#define _GRID

#include "Body.h"
#include "CurrentState.h"
#include "Utils.h"
#include <vector>
#include <iterator>


#define SEGMENT 64
#define NUM_GAUSS_TEXTURES 4

using namespace std;



class Grid : public Body
{
public:
	 GLushort indices[SEGMENT * SEGMENT * 6];

	 void Create();
	 void Destroy();
	 void Draw(int, int);
	 void DrawShadow();

	/* GLuint GetViewMatrixUniformLocation();
	 GLuint GetProjectionMatrixUniformLocation();
	 GLuint GetProgramId();*/

	 GLuint texId;

	ILuint texid;
	ILboolean success; 
	int finished;

	 GLuint 
		 DirectionLightUniformLocation[2],
		 DirectionLightColourUniformLocation[2];
	 Vector DirectLight,
			ViewVect;
	 Vertex DirectLightColour;
	 void SetTexParams(string , GLuint &, GLuint &, GLuint );
	
};

#endif