#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "helperClass.h"
#include <cmath>


class fft
{
private:
	 int _CurrentSize;
	 complex ** computeTittle(int size);
	 unsigned int reverse(unsigned int index);
	 int log_2_N;
	 complex *output;
	 complex ** twittleFactors;
	 int size;
public:
	complex * fft::ComputeFFT( complex * input,int stride, int offset);
	void setup(int inputSize);
};

void  fft::setup(int inputSize)
{
	size = inputSize;
	log_2_N = int(log(float(size))/log(2.0));
	output = new complex[size];
	twittleFactors = computeTittle(size);
}


complex* fft::ComputeFFT( complex * input,int stride, int offset)
{

	// copy the input to the output
	for(int i=0;i<size;i++)
	{
		output[i] = input[reverse(i)*stride +offset];
	}

	int numberOfloops = size/2;
	int iterations = 1;
	for(int i=0;i<log_2_N;i++) // how many stage butterfly
	{
		
		for(int j =0;j<numberOfloops;j++)
		{
			for(int k =0;k<iterations;k++)
			{
				int index1 = j*iterations*2+k;
				int index2= j*iterations*2+iterations+k;
				complex twiddle = twittleFactors[i][k];
				complex tmp1 = output[index1];
				complex tmp2 = output[index2];
				output[index1] = tmp1+tmp2*twiddle;
				output[index2] = tmp1-tmp2*twiddle; 
			}
		}
		numberOfloops /=2;
		iterations*=2;
	}
	for(int i=0;i<size;i++)
	{
		input[i*stride +offset] = output[i];
	}

	return output;
}

complex ** fft::computeTittle(int size)
{
	int pow = 1;
	complex** twittleFactors = new complex*[log_2_N];
	for(int i =0;i<log_2_N;i++)
	{
		twittleFactors[i] = new complex[pow];
		for(int j =0;j<pow;j++)
		{
			complex value;
			float angle = 2*pi * j / (pow*2);
			value.real = cos(angle);
			value.imaginary = -sin(angle);
			twittleFactors[i][j]= value;
		}
		pow *=2;
	}
	return twittleFactors;
}

unsigned int fft::reverse(unsigned int index)
{
	unsigned int res = 0;
    for (int j = 0; j < log_2_N; j++) {
        res = (res << 1) + (index & 1);
        index >>= 1;
    }
    return res;
}