#include "Cube.h"
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include "objLoader.h"
#include <vector>
#include <cmath>

#define PI 3.14159265

map<std::string, Edge> edgeTable;
map<std::string, Edge>::iterator it;
std::vector<Vertex> ShadowVertices;
std::vector<Vertex> VERTICES;
Vertex VERTICESCOPY[530];
std::vector<Vector> surfaceNorm;
std::vector<Vertex> surfaceVertex;
int countUniq = 0;
int extrudedCount = 0;

void printVector(obj_vector *v)
{
	printf("%.2f,", v->e[0] );
	printf("%.2f,", v->e[1] );
	printf("%.2f  ", v->e[2] ); 
} 

void printVector(Vector *v)
{
	printf("%.2f,", v->v[0] );
	printf("%.2f,", v->v[1] );
	printf("%.2f  ", v->v[2] );
} 



void Cube::Create()
{
	ModelMatrix = IDENTITY_MATRIX;   
	TranslateMatrix(&ModelMatrix, 0.0f, 0.0f, 0.0f);
	ScaleMatrix(&ModelMatrix, 30.01f, 30.01f, 30.01f);

	CubeRotation = 0;

	DirectLightColour.Color[0] = 0.0f;
	DirectLightColour.Color[1] = 0.5f;
	DirectLightColour.Color[2] = 0.0f;
	DirectLightColour.Color[3] = 0.1f;

    objLoader *objData = new objLoader();
    //objData->load("Models\\bun_zipper.obj");

     objData->load("Models\\cube.obj");
	
	std::vector<Vector> TexTea;
	float Color[4] = {0,0,0.5,1};
	Vector radius;
	for(int i=0; i<objData->vertexCount; i++)
	{
        Vertex verData;
        verData.Position[0] = 0.01f * (objData->vertexList[ i ])->e[0];
        verData.Position[1] = 0.01f * (objData->vertexList[ i ])->e[1];
        verData.Position[2] = 0.01f * (objData->vertexList[ i ])->e[2];
        verData.Position[3] = 1.0f;
	    verData.Color[0] = Color[0];
        verData.Color[1] = Color[1];
        verData.Color[2] = Color[2];
        verData.Color[3] = Color[3];
		verData.identity = i;
		VERTICES.push_back(verData);

		FindMinAndMax(VERTICES[i].Position[0], 
				VERTICES[i].Position[1], VERTICES[i].Position[2]);
	}

	Xc = (Xmin + Xmax) / 2.0f; 
	Zc = (Zmin + Zmax) / 2.0f;
	float theta;
	float X, Z;
	for(int i=0; i<objData->vertexCount; i++)
	{
		X = (VERTICES[i].Position[0] - Xc);
		Z = (VERTICES[i].Position[2] - Zc);
		if(( X > 0 && Z > 0) || (Z > 0 && X < 0))
			theta = atan2(Z, X);
		if((X < 0 && Z < 0) || (X > 0 && Z < 0))
			theta = (2 * PI) - atan2(-1*Z, X);

        Vector tempTex;
        tempTex.v[0] =  theta/(2 * 3.14159f);
        tempTex.v[1] = (VERTICES[i].Position[1] - Ymax) / (Ymin - Ymax);
		TexTea.push_back(tempTex);
	}

	int ctr = 0;
	std::cout << objData->vertexCount << endl;
	for(int i=0; i<objData->faceCount; i++)
	{
		obj_face *o = objData->faceList[i];
		for(int j=0; j<3; j++)
		{
            ctr++;
			INDICES.push_back(o->vertex_index[j]);
		}
	} 
	//calculate surface normal
	Vertex p1, p2, p3;
	
	Vector a, b;
	
	for(int i = 0; i < ctr; i+=3)
	{
		p1 = VERTICES[INDICES[i]];
		p2 = VERTICES[INDICES[i+1]];
		p3 = VERTICES[INDICES[i+2]];
		
		CreateVector(p1,p2,&a);
		CreateVector(p2,p3,&b);
        Vector result;
		Cross(a, b, &result);
        surfaceNorm.push_back(result);
        Vertex surfVertex = p1;
		FindSurfaceVertex(p1, p2, p3, surfVertex);
        surfaceVertex.push_back(surfVertex);
        countUniq++;
	}
	cout << "Surf count = " << countUniq << endl;
	std::vector<Vector> Normals;
	
	for(int i=0; i<objData->normalCount; i++)
	{
        Vector normal;
        normal.v[0] = (objData->normalList[i])->e[0];
        normal.v[1] = (objData->normalList[i])->e[1];
        normal.v[2] = (objData->normalList[i])->e[2];
		Normalize(&normal);
		Normals.push_back(normal);
	}
    ExitOnGLError("ERROR: before create the shader program");
	ShaderIds[0] = glCreateProgram();
	ExitOnGLError("ERROR: Could not create the shader program");
	
	ShaderIds[1] = LoadShader("Shaders\\AmbientLight.fragment.glsl", GL_FRAGMENT_SHADER);
	ShaderIds[2] = LoadShader("Shaders\\AmbientLight.vertex.glsl", GL_VERTEX_SHADER);
	glAttachShader(ShaderIds[0], ShaderIds[1]);
	glAttachShader(ShaderIds[0], ShaderIds[2]);
	
	glLinkProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not link the shader program");

    /*ShaderIds[3] = glCreateProgram();
    ExitOnGLError("ERROR: Could not create the shader program");

    ShaderIds[4] = LoadShader("Shaders\\Texture.fragment.glsl", GL_FRAGMENT_SHADER);
    ShaderIds[5] = LoadShader("Shaders\\Texture.vertex.glsl", GL_VERTEX_SHADER);
    glAttachShader(ShaderIds[3], ShaderIds[1]);
    glAttachShader(ShaderIds[3], ShaderIds[2]);

    glLinkProgram(ShaderIds[3]);
    ExitOnGLError("ERROR: Could not link the shader program");*/

	int counter = 0;
	for(int i = 0; i < 4; i+=4)
	{
		ModelMatrixUniformLocation[counter] = glGetUniformLocation(ShaderIds[i], "ModelMatrix");
		ViewMatrixUniformLocation[counter] = glGetUniformLocation(ShaderIds[i], "ViewMatrix");
		ProjectionMatrixUniformLocation[counter] = glGetUniformLocation(ShaderIds[i], "ProjectionMatrix");
		DirectionLightUniformLocation[counter] = glGetUniformLocation(ShaderIds[i], "DirectionLight");
		DirectionLightColourUniformLocation[counter] = glGetUniformLocation(ShaderIds[i], "DirectionLightColour");
	//	ViewVectorUniformLocation[counter] = glGetUniformLocation(ShaderIds[i], "ViewVector");
		MVMatrixUniformLocation[counter] = glGetUniformLocation(ShaderIds[i], "MVMatrix");
        ShinyUniformLocation[counter] = glGetUniformLocation(ShaderIds[i], "shiny");
		gaussianTextureUnif[counter] = glGetUniformLocation(ShaderIds[i], "gaussianTexture");
		bumpTextureUnif = glGetUniformLocation(ShaderIds[i], "bumpTexture");

		ExitOnGLError("ERROR: Could not get shader uniform locations -- ");
		counter++;
	}

	glGenVertexArrays(1, &BufferIds[0]);  
	ExitOnGLError("ERROR: Could not generate the VAO");
	glBindVertexArray(BufferIds[0]); 
	ExitOnGLError("ERROR: Could not bind the VAO");

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	ExitOnGLError("ERROR: Could not enable vertex attributes");

	glGenBuffers(2, &BufferIds[1]);
	ExitOnGLError("ERROR: Could not generate the buffer objects");

	glBindBuffer(GL_ARRAY_BUFFER, BufferIds[1]);
	glBufferData(GL_ARRAY_BUFFER, VERTICES.size() * sizeof(Vertex), &VERTICES[0], GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the VBO to the VAO");
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDICES.size() * sizeof(GLuint), &INDICES[0], GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the IBO to the VAO");
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VERTICES[0]), (GLvoid*)sizeof(VERTICES[0].Position));
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glGenBuffers(1, &VertNormBuff);
	ExitOnGLError("ERROR: Could not generate the vertex normal buffer");

	glBindBuffer(GL_ARRAY_BUFFER, VertNormBuff);
	glBufferData(GL_ARRAY_BUFFER, Normals.size() * sizeof(Vector), &Normals[0], GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the Vertex Normals to the VAO");
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Normals[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glGenBuffers(1, &TexBuffer);
	ExitOnGLError("ERROR: Could not generate the texture buffer");
	glBindBuffer(GL_ARRAY_BUFFER, TexBuffer);
	glBufferData(GL_ARRAY_BUFFER, TexTea.size() * sizeof(Vector), &TexTea[0], GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the texture to the VAO");
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, sizeof(TexTea[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");
	
	glBindVertexArray(0);
	string filename = "Textures\\horse.png";
	string bumpyFilename = "Textures\\normal.jpg";
	SetTexParams(filename, teaTex, teaSampler, 0);
	glGenSamplers(1, &teaSampler);
		   glSamplerParameteri(teaSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   glSamplerParameteri(teaSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		   glSamplerParameteri(teaSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		   glSamplerParameteri(teaSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	SetTexParams(bumpyFilename, bumpTex, bumpSampler, 1);
	glGenSamplers(1, &bumpSampler);
		   glSamplerParameteri(bumpSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   glSamplerParameteri(bumpSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		   glSamplerParameteri(bumpSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		   glSamplerParameteri(bumpSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

	width = 1220;
	height = 650;

    glGetError();
	
	CreateShadowPolygons();
}

void Cube::CreateVerticesForQuad(Vector lightPos)
{
	int counter = 0;
//	cout << "Edge table size = " << edgeTable.size() << endl;
    ShadowVertices.clear();
	ShadowINDICES.clear();
	int indexCtr = 0;
	GLuint firstCol[2];
	GLuint lastCol[2];
	for(it = edgeTable.begin(); it != edgeTable.end(); it++)
	{
		counter++;
		/*if(counter <=2)
			continue;*/
		Vertex *result = new Vertex();
		ShadowVertices.push_back(*(it->second.first));
		ShadowINDICES.push_back(indexCtr);
		ShadowVertices.push_back(*(it->second.second));
		ShadowINDICES.push_back(indexCtr+1);

		TranslatePointOnVector(*(it->second.first), lightPos, result, 0.02f);
		ShadowVertices.push_back(*result);
		ShadowINDICES.push_back(indexCtr+2);

		ShadowINDICES.push_back(indexCtr+1);
		ShadowINDICES.push_back(indexCtr+3);
		ShadowINDICES.push_back(indexCtr+2);

		TranslatePointOnVector(*(it->second.second), lightPos, result, 0.02f);
		ShadowVertices.push_back(*result);
		indexCtr += 4;
		
		
	}
	
	for(int i = 0; i < ShadowINDICES.size(); i+=3)
	{
		std::cout << ShadowINDICES[i] << "," << ShadowINDICES[i+1]  << ","  << ShadowINDICES[i+2] << std::endl; 
	}
	extrudedCount = ShadowVertices.size();
}

void Cube::CreateShadowPolygons()
{
    if(state.lightChanged){
        int size  = VERTICES.size();
        int i = 0;
        while(i< size)
        {
            VERTICESCOPY[i] = VERTICES.at(i);
            i++;
        }

        FindSilhouette(surfaceNorm, surfaceVertex,  INDICES, VERTICESCOPY, state.GetLightDirection(), countUniq);
        CreateVerticesForQuad(state.GetLightDirection());
        state.lightChanged = false;
    }
	GLenum ErrorCheckValue = glGetError();
 
	ShadowShaderIds[0] = glCreateProgram();
	ExitOnGLError("ERROR: Could not create the shader program");
	
	ShadowShaderIds[1] = LoadShader("Shaders\\Shadow.fragment.glsl", GL_FRAGMENT_SHADER);
	ShadowShaderIds[2] = LoadShader("Shaders\\Shadow.vertex.glsl", GL_VERTEX_SHADER);
	glAttachShader(ShadowShaderIds[0], ShadowShaderIds[1]);
	ExitOnGLError("ERROR: Could not attach the fragmetn shader program");
	glAttachShader(ShadowShaderIds[0], ShadowShaderIds[2]);
	ExitOnGLError("ERROR: Could not attach the vertex shader program");
	
	glLinkProgram(ShadowShaderIds[0]);
	ExitOnGLError("ERROR: Could not link the shader program");

	ShadowModelMatrixUniformLocation = glGetUniformLocation(ShadowShaderIds[0], "ModelMatrix");
	ExitOnGLError("ERROR: Could not glGetUniformLocation ModelMatrix");
	ShadowViewMatrixUniformLocation = glGetUniformLocation(ShadowShaderIds[0], "ViewMatrix");
	ShadowProjectionMatrixUniformLocation = glGetUniformLocation(ShadowShaderIds[0], "ProjectionMatrix");

    glGenVertexArrays(1, &ShadowBufferIds[0]);
	ExitOnGLError("ERROR: Could not generate VAO");
    glBindVertexArray(ShadowBufferIds[0]);
		ExitOnGLError("ERROR: Could not bind VAO");

		glGenBuffers(2, &ShadowBufferIds[1]);
		ExitOnGLError("ERROR: Could not gen buffers");
		glBindBuffer(GL_ARRAY_BUFFER, ShadowBufferIds[1]);
		ExitOnGLError("ERROR: Could not bind buffers");
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* ShadowVertices.size(), &ShadowVertices[0], GL_STATIC_DRAW);
		ExitOnGLError("ERROR: Could not set buffer data");
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ShadowVertices[0]), (GLvoid*)0);
		ExitOnGLError("ERROR: in glVertexAttribPointer");

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ShadowBufferIds[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, ShadowINDICES.size() * sizeof(GLuint), &ShadowINDICES[0], GL_STATIC_DRAW);
		ExitOnGLError("ERROR: Could not bind the IBO to the VAO");
	//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ShadowVertices[0]), (GLvoid*)sizeof(ShadowVertices[0].Position));
		ExitOnGLError("ERROR: Could not set VAO attributes");

	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glBindVertexArray(0);
 
   /* glGenBuffers(1, &ShadowBufferIds[2]);
    glBindBuffer(GL_ARRAY_BUFFER, ShadowBufferIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* countUniq, &ShadowVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ShadowVertices[0]), (GLvoid*)sizeof(ShadowVertices[0].Position));
    glEnableVertexAttribArray(1);*/
	ErrorCheckValue = glGetError();
}


void Cube::FindSilhouette(std::vector<Vector> surfaceNormal, std::vector<Vertex> surfaceVertex, 
					  std::vector<GLuint> indexBuf, Vertex* vertices, Vector lightPos, int size)
{
	Vector light;
	float dot = 0;
	int index = 0;
	Vertex *v1, *v2, *v3;
	string e1, e2, e3;
	//find light dir
	edgeTable.clear();
	for(int i = 0; i < size; i++)
	{
		CreateVector(surfaceVertex[i], lightPos, &light);
		Normalize(&light);
		//cross product of light dir and surf normal
		dot = Dot(light, surfaceNormal[i]);
		float theta = acos (dot) * 180.0 / PI;
		std::cout << "Theta - " << theta << std::endl;
		if(theta >= 0 && theta <= 90)
		//if(dot > 0)
		{
			index = i * 3;
			//find the 3 vertices of this triangle
           
            v1 = &vertices[indexBuf[index]];
            v2 = &vertices[indexBuf[index+1]];
            v3 = &vertices[indexBuf[index+2]];
			//create the edge key: low vertex id - high vertex id
			e1 = (v1->identity < v2->identity ? FormEdgeHash(v1->identity, v2->identity) : FormEdgeHash(v2->identity, v1->identity));
			e2 = (v2->identity < v3->identity ? FormEdgeHash(v2->identity, v3->identity) : FormEdgeHash(v3->identity, v2->identity));
			e3 = (v1->identity < v3->identity ? FormEdgeHash(v1->identity, v3->identity) : FormEdgeHash(v3->identity, v1->identity));

			Edge ed1 = { v1, v2 };
			Edge ed2 = { v2, v3 };
			Edge ed3 = { v1, v3 };
			
			it = edgeTable.find(e1);
			if(it != edgeTable.end())
				edgeTable.erase(e1);
			else
				edgeTable[e1] = ed1;

			it = edgeTable.find(e2);
			if(it != edgeTable.end())
				edgeTable.erase(e2);
			else
				edgeTable[e2] = ed2;

			it = edgeTable.find(e3);
			if(it != edgeTable.end())
				edgeTable.erase(e3);
			else
				edgeTable[e3] = ed3;
		}
	}
	cout << edgeTable.size() << endl;
}


void Cube::ColourSilhouette()
{
	for(it = edgeTable.begin(); it != edgeTable.end(); it++)
	{
		it->second.first->Color[0] = 1.0f;
	}
}

string Cube::FormEdgeHash(int e1, int e2)
{
   std::stringstream ss;
   ss << e1;
   ss << "-";
   ss << e2;
   return ss.str();
}

void Cube::SetTexParams(string filename, GLuint &imageHandle, GLuint &sampler, GLuint offset)
{
	GLuint texId;
	ilInit(); /* Initialization of DevIL */
	ilGenImages(1, &texid); /* Generation of one image name */
	ilBindImage(texid); /* Binding of image name */
	
	bool  success = ilLoadImage((const ILstring)filename.c_str()); /* Loading of image "image.jpg" */

    if (success) /* If no error occured: */
	{
	   cout << "\n Loaded image\n";
	   success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); /* Convert every colour component into
		 unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */

	   glGenTextures(1, &imageHandle); /* Texture name generation */
	   glActiveTexture(GL_TEXTURE0+offset);
	   glBindTexture(GL_TEXTURE_2D, imageHandle); /* Binding of texture name */

	   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	  
	   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
		 ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
		 ilGetData()); /* Texture specification */
	  
	   glBindSampler(0, sampler);
	   ilDeleteImages(1, &texid);
	}
	else
		cout << "\n Failed to load image\n";
}

void Cube::FindMinAndMax(float x, float y, float z)
{
	if ( x > Xmax)
		Xmax = x;
	if ( x < Xmin)
		Xmin = x;
	if ( y > Ymax)
		Ymax = y;
	if ( y < Ymin)
		Ymin = y;
	if ( z > Zmax)
		Zmax = z;
	if ( z < Zmin)
		Zmin = z;
}


void Cube::Destroy()
{
	glDetachShader(ShaderIds[0], ShaderIds[1]);
	glDetachShader(ShaderIds[0], ShaderIds[2]);
	glDeleteShader(ShaderIds[1]);
	glDeleteShader(ShaderIds[2]);
	glDeleteProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not destroy the shaders");

	glDeleteBuffers(1, &BufferIds[1]);
	glDeleteVertexArrays(1, &BufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");


	glDetachShader(ShadowShaderIds[0], ShadowShaderIds[1]);
	glDetachShader(ShadowShaderIds[0], ShadowShaderIds[2]);
	glDeleteShader(ShadowShaderIds[1]);
	glDeleteShader(ShadowShaderIds[2]);
	glDeleteProgram(ShadowShaderIds[0]);
	ExitOnGLError("ERROR: Could not destroy the shaders");

	glDeleteBuffers(1, &ShadowBufferIds[1]);
	glDeleteVertexArrays(1, &ShadowBufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");
}

void Cube::Draw(int shader, int location)
{
	/*float CubeAngle;
	clock_t Now = clock();
	if (state.GetLastTime() == 0)
		state.SetLastTime(Now);
	CubeRotation += 45.0f * (float)(Now - state.GetLastTime()) / CLOCKS_PER_SEC;
	CubeRotation = 0;
	CubeAngle = DegreesToRadians(CubeRotation);
	state.SetLastTime(Now); */
    location = 0;
	if(shader == 3)
	{
		/*glDepthMask(0);*/
		glDepthFunc(GL_EQUAL);
       // glDisable(GL_DEPTH_TEST);
        /*glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 0x0, 0xff);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);*/
	}
    else
        glDepthFunc(GL_LESS);
	glUseProgram(ShaderIds[0]);
		ExitOnGLError("ERROR: Could not use the shader program - Cube");

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);

		ViewVect.v[0] = state.GetViewMatrix().m[2];
		ViewVect.v[1] = state.GetViewMatrix().m[6];
		ViewVect.v[2] = state.GetViewMatrix().m[10];

		glUniformMatrix4fv(ModelMatrixUniformLocation[location], 1, GL_FALSE, ModelMatrix.m);
		glUniformMatrix4fv(ViewMatrixUniformLocation[location], 1, GL_FALSE, state.GetViewMatrix().m);
		glUniformMatrix4fv(ProjectionMatrixUniformLocation[location], 1, GL_FALSE, state.GetProjectionMatrix().m);
		glUniform3fv(DirectionLightUniformLocation[location], 1, (state.GetLightDirection()).v);
		glUniform4fv(DirectionLightColourUniformLocation[location], 1, DirectLightColour.Color);
	//	glUniform3fv(ViewVectorUniformLocation[location], 1, ViewVect.v);
        bool shiny = false;
        if (shader == 3)
            shiny = true;
        glUniform1i(ShinyUniformLocation[location], shiny);
		glUniform1i(gaussianTextureUnif[location], 0);
		glUniform1i(bumpTextureUnif, 1);
	  
		Matrix MV = MultiplyMatrices(&(state.GetViewMatrix()), &ModelMatrix);
		Matrix MVInv = TransposeMatrix(&(InverseMatrix(&MV)));
		glUniformMatrix4fv(MVMatrixUniformLocation[location], 1, GL_FALSE, MVInv.m);
		ExitOnGLError("ERROR: Could not set the shader uniforms");
		//glEnable(GL_MULTISAMPLE);
		
		glBindVertexArray(BufferIds[0]);
			ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, teaTex); /* Binding of texture name */
			glBindSampler(0, teaSampler);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, bumpTex); /* Binding of texture name */
			glBindSampler(0, bumpSampler);
			
				glDrawElements(GL_TRIANGLES, INDICES.size() * sizeof(GLuint), GL_UNSIGNED_INT, (GLvoid*)0);
			
			ExitOnGLError("ERROR: Could not draw the cube");
			glBindTexture(GL_TEXTURE_2D, 0); /* Binding of texture name */
		glBindVertexArray(0);
	glUseProgram(0);
	if(shader == 3)
	{
		/*glDepthMask(1);
		glDepthFunc(GL_LESS);*/
	}
	
}

void Cube::DrawShadow()
{
	if(state.lightChanged){
        int size  = VERTICES.size();
        int i = 0;
        while(i< size)
        {
            VERTICESCOPY[i] = VERTICES.at(i);
            i++;
        }

		FindSilhouette(surfaceNorm, surfaceVertex,  INDICES, VERTICESCOPY, state.GetLightDirection(), countUniq);
		CreateVerticesForQuad(state.GetLightDirection());
		state.lightChanged = false;
	}
	//UPDATE THE BUFFER INFORMATION
    glDepthFunc(GL_LESS);
	glBindBuffer(GL_ARRAY_BUFFER, ShadowBufferIds[1]);
	ExitOnGLError("ERROR: Could not bind buffers");
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* countUniq, &ShadowVertices[0], GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not set buffer data");

	glEnable(GL_STENCIL_TEST);
	glClearStencil(0);
	glStencilFunc(GL_ALWAYS, 0, 1);

	glUseProgram(ShadowShaderIds[0]);  
	ExitOnGLError("use");
    glUniformMatrix4fv(ShadowModelMatrixUniformLocation, 1, GL_FALSE, ModelMatrix.m);
    ExitOnGLError("glUniformMatrix4fv ModelMat");
    glUniformMatrix4fv(ShadowViewMatrixUniformLocation, 1, GL_FALSE, state.GetViewMatrix().m);
    ExitOnGLError("glUniformMatrix4fv ViewMat");
    glUniformMatrix4fv(ShadowProjectionMatrixUniformLocation, 1, GL_FALSE, state.GetProjectionMatrix().m);
    ExitOnGLError("glUniformMatrix4fv ProjMat");
    glBindVertexArray(ShadowBufferIds[0]);

	if(state.drawExtruded)
		glColorMask(0,0,0,0); //disable all writing to color buffer

    glCullFace(GL_BACK);
    glStencilFunc(GL_ALWAYS, 0x0, 0xff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
    glFrontFace(GL_CCW);
    //glDrawArrays(GL_TRIANGLES, 0, extrudedCount);
	glDrawElements(GL_TRIANGLES, ShadowINDICES.size() * sizeof(GLuint), GL_UNSIGNED_INT, (GLvoid*)0);
    ExitOnGLError("draw");
	//	glColorMask(1,1,1,1);
			
	glCullFace(GL_FRONT);
	glStencilFunc(GL_ALWAYS, 0x0, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
//	glFrontFace(GL_CCW);
	glDrawElements(GL_TRIANGLES, ShadowINDICES.size() * sizeof(GLuint), GL_UNSIGNED_INT, (GLvoid*)0);
	//glDrawArrays(GL_TRIANGLES, 0, extrudedCount);
	ExitOnGLError("draw");
	if(state.drawExtruded)
		glColorMask(1,1,1,1);

/*	glStencilFunc(GL_EQUAL, 0x0, 0xff);
	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, countUniq);
	ExitOnGLError("draw");*/
		
	glBindVertexArray(0);
	glUseProgram(0);
	
	glDisable(GL_STENCIL_TEST);
}



//GLuint Cube::GetViewMatrixUniformLocation()
//{
//	return ViewMatrixUniformLocation[0];
//}
//
//GLuint Cube::GetProjectionMatrixUniformLocation()
//{
//	return ProjectionMatrixUniformLocation[0];
//}
//
//GLuint Cube::GetProgramId()
//{
//	return ShaderIds[0];
//}
//
