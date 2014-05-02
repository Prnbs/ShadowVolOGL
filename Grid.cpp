#include "Grid.h"
#include <cstring>

void Grid::Create()
{
	DirectLightColour.Color[0] = 0.9f;
	DirectLightColour.Color[1] = 0.0f;
	DirectLightColour.Color[2] = 0.0f;
	DirectLightColour.Color[3] = 0.1f;
    for(int i = 0; i < 6; i++)
     ShaderIds[i] = 20;
    for(int i= 0 ; i < 2; i ++)
    {
        ModelMatrixUniformLocation[i] = 20;
        ViewMatrixUniformLocation[i] = 20;
        ProjectionMatrixUniformLocation[i] = 20;
        DirectionLightUniformLocation[i] = 20;
        DirectionLightColourUniformLocation[i] = 20;
        ViewVectorUniformLocation[i] = 20;
        MVMatrixUniformLocation[i] = 20;
        gaussianTextureUnif[i] = 20;
    }

	Vertex Grid[(SEGMENT+2) * (SEGMENT+2)]; 
	Vector Tex[(SEGMENT+2) * (SEGMENT+2)];
	int n = 0;
	for(int i = 0; i <= SEGMENT; i++)
	{
		for(int j = 0; j <= SEGMENT; j++)
		{
			Grid[n].Position[0] = (float)i - SEGMENT/2;
			Grid[n].Position[1] = -1.0f;
			Grid[n].Position[2] = (float)j - SEGMENT/2;
			Grid[n].Position[3] = 1.0f;
			//cout << "X = " << (float)i / (float)SEGMENT << ", Y = " << (float)j / (float)SEGMENT << endl;
		
			Grid[n].Color[0] = 1.0f;
			Grid[n].Color[1] = 0.0f;
			Grid[n].Color[2] = 0.0f;
			Grid[n++].Color[3] = 1.0f;

			Tex[n].v[0] =  (float)i / (float)SEGMENT;
			Tex[n].v[1] = (float)j / (float)SEGMENT;
		
		}
	}
	cout << "N = " << n << endl;

	ShaderIds[0] = glCreateProgram();
	ExitOnGLError("ERROR: Could not create the shader program");
    ShaderIds[1] = LoadShader("Shaders\\AmbientLight.fragment.glsl", GL_FRAGMENT_SHADER);
    ShaderIds[2] = LoadShader("Shaders\\AmbientLight.vertex.glsl", GL_VERTEX_SHADER);
    /*  ShaderIds[1] = LoadShader("Shaders\\Texture.fragment.glsl", GL_FRAGMENT_SHADER);
    ShaderIds[2] = LoadShader("Shaders\\Texture.fragment.glsl", GL_VERTEX_SHADER);*/
	glAttachShader(ShaderIds[0], ShaderIds[1]);
	glAttachShader(ShaderIds[0], ShaderIds[2]);
	
	glLinkProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Could not link the shader program - Amb");

	//ShaderIds[3] = glCreateProgram();
	//ExitOnGLError("ERROR: Could not create the shader program");
	//
 //   ShaderIds[4] = LoadShader("Shaders\\Texture.fragment.glsl", GL_FRAGMENT_SHADER);
 //   ShaderIds[5] = LoadShader("Shaders\\Texture.fragment.glsl", GL_VERTEX_SHADER);
	//glAttachShader(ShaderIds[3], ShaderIds[1]);
	//glAttachShader(ShaderIds[3], ShaderIds[2]);
	//
	//glLinkProgram(ShaderIds[3]);
	//ExitOnGLError("ERROR: Could not link the shader program- Tex");
	int count = 0;
	for(int i = 0; i < 4; i+=4)
	{
		ModelMatrixUniformLocation[count] = glGetUniformLocation(ShaderIds[i], "ModelMatrix");
        ExitOnGLError("ERROR: Could not get shader uniform locations -- ModelMatrix");
		ViewMatrixUniformLocation[count] = glGetUniformLocation(ShaderIds[i], "ViewMatrix");
        ExitOnGLError("ERROR: Could not get shader uniform locations -- ViewMatrix");
		ProjectionMatrixUniformLocation[count] = glGetUniformLocation(ShaderIds[i], "ProjectionMatrix");
        MVMatrixUniformLocation[count] = glGetUniformLocation(ShaderIds[i], "MVMatrix");
        ExitOnGLError("ERROR: Could not get shader uniform locations -- MVMatrixUniformLocation");
		DirectionLightUniformLocation[count] = glGetUniformLocation(ShaderIds[i], "DirectionLight");
        ExitOnGLError("ERROR: Could not get shader uniform locations -- DirectionLightUniformLocation");
        DirectionLightColourUniformLocation[count] = glGetUniformLocation(ShaderIds[i], "DirectionLightColour");
        ExitOnGLError("ERROR: Could not get shader uniform locations -- DirectionLightColour");
         /*ViewVectorUniformLocation[count] = glGetUniformLocation(ShaderIds[i], "ViewVector");
         ExitOnGLError("ERROR: Could not get shader uniform locations -- ViewVectorUniformLocation");*/
		ShinyUniformLocation[count] = glGetUniformLocation(ShaderIds[i], "shiny");
		gaussianTextureUnif[count] = glGetUniformLocation(ShaderIds[i], "gaussianTexture");
         ExitOnGLError("ERROR: Could not get shader uniform locations -- gaussianTextureUnif");
		ExitOnGLError("ERROR: Could not get shader uniform locations -- ");
		count++;
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(Grid), Grid, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the VBO to the VAO");

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Grid[0]), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Grid[0]), (GLvoid*)sizeof(Grid[0].Position));
	ExitOnGLError("ERROR: Could not set VAO attributes");

	
	int k = 0;
	for(GLushort i = 0; i < SEGMENT; i++)
	{
		for(GLushort j = 0;j < SEGMENT; j++)
		{
			indices[k++] = i * (SEGMENT+1) + j;
			indices[k++] = i * (SEGMENT+1) + j + 1;
			indices[k++] = (i + 1) * (SEGMENT+1) + j;

			indices[k++] = i * (SEGMENT+1) + j + 1;
			indices[k++] = (i + 1) * (SEGMENT+1) + j + 1;
			indices[k++] = (i + 1) * (SEGMENT+1) + j;
		}
	}

	Vector Normals[(SEGMENT+1) * (SEGMENT+1)];// = {{0,1,0}};
	for(int i = 0; i < (SEGMENT+1) * (SEGMENT+1); i++)
	{
		Normals[i].v[0] = 0.0;
		Normals[i].v[1] = 10.0;
		Normals[i].v[2] = 0.0;
	}

	cout << "Index size = " << k << endl;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BufferIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the IBO to the VAO");

	glGenBuffers(1, &VertNormBuff);
	ExitOnGLError("ERROR: Could not generate the vertex normal buffer");

	glBindBuffer(GL_ARRAY_BUFFER, VertNormBuff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals), Normals, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the Vertex Normals to the VAO");
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Normals[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glGenBuffers(1, &TexBuffer);
	ExitOnGLError("ERROR: Could not generate the texture buffer");
	glBindBuffer(GL_ARRAY_BUFFER, TexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tex), Tex, GL_STATIC_DRAW);
	ExitOnGLError("ERROR: Could not bind the texture to the VAO");
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Tex[0]), (GLvoid*)0);
	ExitOnGLError("ERROR: Could not set VAO attributes");

	glBindVertexArray(0);

	string filename = "Textures\\Jellyfish.jpg";
	
	glGenSamplers(1, &g_gaussSampler);
		   glSamplerParameteri(g_gaussSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		   glSamplerParameteri(g_gaussSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		   glSamplerParameteri(g_gaussSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		   glSamplerParameteri(g_gaussSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    SetTexParams(filename, image, g_gaussSampler, 0);

	ExitOnGLError("ERROR: problems setting up floor texture");
}

void Grid::SetTexParams(string filename, GLuint &imageHandle, GLuint &sampler, GLuint offset)
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
       ExitOnGLError("ERROR: problems setting up in SetTexParams glGenTextures");

	   glActiveTexture(GL_TEXTURE0+offset);
	   glBindTexture(GL_TEXTURE_2D, imageHandle); /* Binding of texture name */
       ExitOnGLError("ERROR: problems setting up in SetTexParams glBindTexture");

	   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	   ExitOnGLError("ERROR: problems setting up in SetTexParams glTexParameterf");

	   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
		 ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
		 ilGetData()); /* Texture specification */
       ExitOnGLError("ERROR: problems setting up in SetTexParams glTexImage2D");
	   glBindSampler(0, sampler);
       ExitOnGLError("ERROR: problems setting up in SetTexParams");
	   ilDeleteImages(1, &texid);
	}
	else
		cout << "\n Failed to load image\n";
}

void Grid::Destroy()
{
	glDetachShader(ShaderIds[0], ShaderIds[1]);
	glDetachShader(ShaderIds[0], ShaderIds[2]);
	glDeleteShader(ShaderIds[1]);
	glDeleteShader(ShaderIds[2]);
	glDeleteProgram(ShaderIds[0]);
	ExitOnGLError("ERROR: Coul d not destroy the shaders");

	glDeleteBuffers(2, &BufferIds[1]);
	glDeleteVertexArrays(1, &BufferIds[0]);
	ExitOnGLError("ERROR: Could not destroy the buffer objects");

}

void Grid::Draw(int shader, int location)
{
	ModelMatrix = IDENTITY_MATRIX;
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	//glDepthMask(0);
	if(shader == 3)
	{
		//glDepthMask(0);
		glDepthFunc(GL_EQUAL);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 0x0, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
	//glEnable(GL_MULTISAMPLE);
    location = 0;
	glUseProgram(ShaderIds[0]);
		ExitOnGLError("ERROR: Could not use the shader program");

		glUniformMatrix4fv(ModelMatrixUniformLocation[location], 1, GL_FALSE, ModelMatrix.m);
		glUniformMatrix4fv(ViewMatrixUniformLocation[location], 1, GL_FALSE, state.GetViewMatrix().m);
		glUniformMatrix4fv(ProjectionMatrixUniformLocation[location], 1, GL_FALSE, state.GetProjectionMatrix().m);
		glUniform3fv(DirectionLightUniformLocation[location], 1, (state.GetLightDirection()).v);
		glUniform4fv(DirectionLightColourUniformLocation[location], 1, DirectLightColour.Color);
	//	glUniform3fv(ViewVectorUniformLocation[location], 1, ViewVect.v);
     //   std::cout << location << " - " << DirectionLightUniformLocation[location] << std::endl;
		glUniform1i(gaussianTextureUnif[location], 0);
        bool shiny = false;
        if (shader == 3)
            shiny = true;
        glUniform1i(ShinyUniformLocation[location], shiny);
		Matrix MV = MultiplyMatrices(&(state.GetViewMatrix()), &ModelMatrix);
		Matrix MVInv = TransposeMatrix(&(InverseMatrix(&MV)));
		glUniformMatrix4fv(MVMatrixUniformLocation[location], 1, GL_FALSE, MVInv.m);
		ExitOnGLError("ERROR: Could not set the shader uniforms -- Grid");

		glBindVertexArray(BufferIds[0]);
			ExitOnGLError("ERROR: Could not bind the VAO for drawing purposes");

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, image); /* Binding of texture name */
			glBindSampler(1, g_gaussSampler);
			//glEnable(GL_MULTISAMPLE_ARB);
				glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_SHORT, (GLvoid*)0);
			//glDisable(GL_MULTISAMPLE_ARB);
			ExitOnGLError("ERROR: Could not draw the grid");
			glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
	glUseProgram(0);
	if(shader == 3)
	{
		//glDepthMask(1);
		glDepthFunc(GL_LESS);
		glDisable(GL_STENCIL_TEST);
	}
	//glDisable(GL_MULTISAMPLE);
}

void Grid::DrawShadow()
{
}

