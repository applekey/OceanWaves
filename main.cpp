#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



#include "helperClass.h"
#include "controls.h"
#include "common.h"
#include "wave.h"
#include "fft.h"

int checkFBOBinding()
{
	GLenum e = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (e != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("There is a problem with the FBO\n");
		switch(e)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			printf("here");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		printf("here");
		break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		printf("here");
		break;
			
		case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("here");
			break;
		default:
			printf("here");
			break;
		}
		return -1;
	}
	else
	{
		return 0;
	}
}

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
int main(void)
{
	glewExperimental=GL_TRUE; 
	glfwInit();

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
	{
		printf("glfw initilizatio nerror");
		 return -1;
	}
      
    window = glfwCreateWindow(1280, 720, "Simple example", NULL, NULL);
    if (!window)
	{
		printf("glfw initilizatio nerror");
			return -1;
	}
	 glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

	GLenum  error = glewInit();
	if(error!=GLEW_OK)
	{
		printf("glew problem\n");
 
	}

	initialViewSetup(window);
	printf("%s\n", glGetString(GL_VERSION));
	// set up the compute shader
	WaveCompute waveCompute;
	waveCompute.Setup( A,N,glm::vec2(WINDX,WINDY));
	// set up the display program
	GLuint displayProgram = LoadShaders( "..\\display.vert","..\\display.frag" );
	GLuint MVPID = glGetUniformLocation(displayProgram, "MVP");
	GLuint data = glGetUniformLocation(displayProgram, "data");
	// generate number of verticies as well as their uv coordinates
	int mrows = OCEAN_SIZE;
	int mcols = OCEAN_SIZE;
	GLfloat* g_vertex_buffer_data = new GLfloat[mrows*mcols*3];
	
	int indexBuffer = 0;
	for(int r = 0;r<mrows; r ++)
	{
		for(int c =0; c<mcols;c++)
		{
			GLfloat vxCoord =(GLfloat)c/(GLfloat)(mcols-1);
			GLfloat vyCoord = 1.0;
			GLfloat vzCoord = (GLfloat)r/(GLfloat)(mrows-1);
			g_vertex_buffer_data[indexBuffer] = vxCoord;
			g_vertex_buffer_data[indexBuffer +1] = vyCoord;
			g_vertex_buffer_data[indexBuffer +2] = vzCoord;
			indexBuffer +=3;
		}
	}
	GLfloat* g_vertex_buffer_data_x = new GLfloat[mrows*mcols];
	GLfloat* g_vertex_buffer_data_z = new GLfloat[mrows*mcols];
	for (int i = 0; i < mrows*mcols*3; i+=3) {
		g_vertex_buffer_data_x[i/3] = g_vertex_buffer_data[i];
	}
	for (int i = 2; i < mrows*mcols*3; i+=3) {
		g_vertex_buffer_data_z[(i-2)/3] = g_vertex_buffer_data[i];
	}
	// generate the indicies
	int stopIndex = (mcols*2+1)*(mrows-1)-1;
	int* iIndices = new int[stopIndex];//have to had each row for the restart
  	int iIndex = 0;
	for(int r=0;r<(mrows -1);r++)
	{
		int c;
		for(c =0; c<(mcols +1);c ++)
		{
			if(iIndex == stopIndex)
				break;
			if(c == mcols )
			{
				iIndices[iIndex] = mcols*mrows;
				iIndex++;
			}
			else
			{
				iIndices[iIndex] = mcols*r+c;
				iIndices[iIndex+1] =mcols*(r+1)+c;
				iIndex +=2;
			}
		}
	}

		// draw the first mesh
	GLuint  mesh,meshData,meshIndicies;

	glGenVertexArrays(1, &mesh); // Create one VAO
	glGenBuffers(1, &meshData); // One VBO for data
	glGenBuffers(1, &meshIndicies); // And finally one VBO for indices

	glBindVertexArray(mesh);
	glBindBuffer(GL_ARRAY_BUFFER, meshData);
	int buffSize = sizeof(GLfloat) *mrows*mcols*3;
	glBufferData(GL_ARRAY_BUFFER, buffSize,g_vertex_buffer_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &meshIndicies);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndicies);
	int indexSize = sizeof(int) * stopIndex;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, iIndices, GL_STATIC_DRAW);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(mcols*mrows);
	

	fft fftWave;
	fftWave.setup(N);

	// create the store texture for the ffted data
	GLuint storeTex;
	glGenTextures(1, &storeTex);
	glBindTexture(GL_TEXTURE_2D, storeTex);
	glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	float currentTime = 2.0;
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE  );
    while (!glfwWindowShouldClose(window))
    {
		currentTime+=0.05;
        //glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
			
		glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

		// calculate the wave parmaters

		complex* output = waveCompute.ComputeCurrent(currentTime);
		// do butterfly ffts
		// do the first the horizontal
		for(int i =0;i<N;i++)
		{
			fftWave.ComputeFFT(output,1,i*N);
		}

		// now do the verticle
		for(int i =0;i<N;i++)
		{
			fftWave.ComputeFFT(output,N,i);
		}
		
		for(int i=0;i<N*N;i++)
		{
			float x = pow(output[i].real,2);
			float y = pow(output[i].imaginary,2);
			output[i].real =sqrt(x+y);
			output[i].imaginary = -1;
			output[i].filler1 = -2;
			output[i].filler2 = -3;

		}
	

		glUseProgram(displayProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, storeTex);
		/*indexBuffer = 0;
			for(int r = 0;r<mrows; r ++)
		{
			for(int c =0; c<mcols;c++)
			{
				GLfloat vxCoord =(GLfloat)c/(GLfloat)(mcols-1);
				GLfloat vyCoord = output[r*N+c].real;
				GLfloat vzCoord = (GLfloat)r/(GLfloat)(mrows-1);
				g_vertex_buffer_data[indexBuffer] = vxCoord;
				g_vertex_buffer_data[indexBuffer +1] = vyCoord;
				g_vertex_buffer_data[indexBuffer +2] = vzCoord;
				indexBuffer +=3;
			}
		}*/

	/*	glBindVertexArray(mesh);
		glBindBuffer(GL_ARRAY_BUFFER, meshData);
		glBufferData(GL_ARRAY_BUFFER, buffSize,g_vertex_buffer_data, GL_STATIC_DRAW);*/



		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, N, N, GL_RGBA, GL_FLOAT, (char *)output);

		computeMatricesFromInputs();
		ProjectionMatrix = getProjectionMatrix();
		ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
			// centering matrix
		glm::mat4 ViewTranslate = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(-0.5f, 0.0f, -0.5));
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix *ViewTranslate*ModelMatrix;
		glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);


		glBindVertexArray(mesh);
		glEnableVertexAttribArray(0);
		glDrawElements(GL_TRIANGLE_STRIP, stopIndex, GL_UNSIGNED_INT, 0);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);

		
		glUseProgram(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
