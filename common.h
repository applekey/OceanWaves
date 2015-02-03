#define OCEAN_SIZE 128
#define A 0.000000002
#define L 128
#define N 128
#define WINDX 1.5
#define WINDY 0.6
#define pi 3.14159265359
typedef struct complexValue
{
	float real;
	float imaginary;
	float filler1;
	float filler2;
	complexValue operator *(const complexValue c2)  
	{  
		complexValue c3;  
		c3.real=(real*c2.real)-(imaginary*c2.imaginary);  
		c3.imaginary=(imaginary*c2.real)+(real*c2.imaginary);  
		return c3;  
	}  

	complexValue operator +(const complexValue c2)  
	{  
		complexValue c3;  
		c3.real=(real+c2.real);
		c3.imaginary=imaginary+c2.imaginary;
		return c3;  
	}  

	complexValue operator -(const complexValue c2)  
	{  
		complexValue c3;  
		c3.real=(real-c2.real);
		c3.imaginary=imaginary-c2.imaginary;
		return c3;  
	}  

}complex;

#define FFTSPLIT 8
//#define DEBUG 0