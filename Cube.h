#ifndef _CUBE
#define _CUBE

#include "Body.h"
#include "CurrentState.h"
#include "Utils.h"
#include <string>

class Cube : public Body
{
public:
	 GLuint 
		 DirectionLightUniformLocation[2],
		 DirectionLightColourUniformLocation[2],
		 ShadowProjectionMatrixUniformLocation,
		 ShadowViewMatrixUniformLocation,
		 ShadowModelMatrixUniformLocation;
	 Vector DirectLight,
			ViewVect;
	GLuint bumpTex, teaTex;
	GLuint bumpSampler, teaSampler;
	GLuint WorldCameraPosUniformLoc;
	GLuint CubeMapTexUniLoc;
	GLuint bumpTextureUnif;
	GLuint ShadowBufferIds[3];
	GLuint ShadowShaderIds[3];
	GLuint outRenderbuffer, outFramebuffer;
	bool lightChanged;
	int err;
    
	ILuint texid;
	ILboolean success; 
	int finished;
	float Xmin;
	float Xmax;
	float Ymin;
	float Ymax;
	float Zmin;
	float Zmax;
	float Xc, Zc;

	GLsizei height, width, format;

	 Vertex DirectLightColour;
	 GLuint INDICES[1024 * 3];
	 int vert;
	 float CubeRotation;
	 void Create();
	 void Destroy();
	 void Draw(int, int);
	 void DrawShadow();
	 void RenderFunction();
	 void CreateShadowPolygons();
	/* GLuint GetViewMatrixUniformLocation();
	 GLuint GetProjectionMatrixUniformLocation();
	 GLuint GetProgramId();*/
	 void FindMinAndMax(float, float, float);
	 void SetTexParams(string , GLuint &, GLuint &,GLuint );
	 string FormEdgeHash(int e1, int e2);
	 void FindSilhouette(Vector* surfaceNormal, Vertex* surfaceVertex, 
					  GLuint* indexBuf, Vertex* vertices, Vector lightPos, int size);
	 void ColourSilhouette();
	 void CreateVerticesForQuad(Vector lightPos);
};

#endif