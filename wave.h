#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "helperClass.h"

class WaveCompute
{
private:
	float _A;
	int _N;
	glm::vec2 _WindDirection;
	GLuint  FSSbo;//input/outputbuffer
	GLuint _calculateHProgram;
	glm::vec3 dimensions;
	GLuint varsA; 
	GLuint varsN;
	GLuint varswindDir;
	GLuint timevar;
	GLuint varsL;

public:
	void Setup( float a, int n, glm::vec2 windDirection);
	complex* ComputeCurrent(float time);
};

void WaveCompute::Setup(float a, int n, glm::vec2 windDirection)
{
	// check the size of n
	assert(n%32 ==0 && "must be size of 32");

	_A = a;
	_N = n;
	_WindDirection = windDirection;
		  
	glGenBuffers( 1, &FSSbo);
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, FSSbo);
	int size = n*n;
	// intlize everthing to -1;
	complex *  intilizeData = new complex[size];
	for(int i =0;i<size;i++)
	{
		intilizeData[i].real = -1.0;
		intilizeData[i].imaginary = -1.0;
	}
	glBufferData( GL_SHADER_STORAGE_BUFFER, size * sizeof(complex), intilizeData, GL_DYNAMIC_DRAW );

	glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );

	_calculateHProgram  = LoadComputeShader( "..\\compute.vert" );
	glBindBufferBase( GL_SHADER_STORAGE_BUFFER,  4,  FSSbo );//position in Compute Shader

	varsL = glGetUniformLocation(_calculateHProgram, "vars.length");
	varsA = glGetUniformLocation(_calculateHProgram, "vars.A");
	varsN = glGetUniformLocation(_calculateHProgram, "vars.N");
	varswindDir = glGetUniformLocation(_calculateHProgram, "vars.windDir");
	timevar = glGetUniformLocation(_calculateHProgram, "currentTime");
	dimensions = glm::vec3(n/32,n/32,1);

}

complex* WaveCompute::ComputeCurrent(float time)
{
	glUseProgram( _calculateHProgram );

	glUniform1f(varsL,_N);
	glUniform1f(varsA,_A);
	glUniform1i(varsN,_N);
	glUniform2f(varswindDir,_WindDirection.x,_WindDirection.y);
	glUniform1f(timevar,time);

	glDispatchCompute(dimensions.x, dimensions.y,  dimensions.z );
	glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, FSSbo );
    //return computed data from shader
	complex *values = (complex*)glMapBuffer( GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
  
	glUnmapBuffer( GL_SHADER_STORAGE_BUFFER );

	
	glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 );
	glUseProgram( 0 );
	return values;
}

