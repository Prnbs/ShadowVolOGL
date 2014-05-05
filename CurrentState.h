#ifndef _CURRSTATE
#define _CURRSTATE
#include "Utils.h"

class CurrentState
{
public:
	static Vector Eye,
				  LookAt;
	static Matrix 
				ProjectionMatrix,
				ViewMatrix;
	static clock_t 
				LastTime;
	static Vector LightDir;
    static bool lightChanged;
	static bool drawExtruded;

    Vector GetEye();
	Vector GetLookAt();
	Matrix GetProjectionMatrix();
	Matrix GetViewMatrix();
	clock_t GetLastTime();
	Vector GetLightDirection();

	void SetEye(Vector);
	void SetLookAt(Vector);
	void SetProjectionMatrix(Matrix);
	void SetViewMatrix(Matrix);
	void SetLastTime(clock_t);
	void SetProgramId(GLuint);
	void SetLightDirection(Vector);
};

#endif