#version 430
#extension GL_ARB_shader_storage_buffer_object : enable

#define M_PI 3.1415926535897932384626433832795
#define GRAV 9.81
precision highp float;

layout( std140, binding=4 ) buffer Buff {
	vec4 F[ ];  //write buffer
};

layout (local_size_x = 32, local_size_y = 32,local_size_z = 1) in;
//////////////////////////////////////////////////////////////////////////////

uniform float currentTime;

struct commonVf
{
	float length;
	float A;
	int N;
	vec2 windDir;
};

uniform commonVf vars;

vec2 complexMulitply(vec2 a, vec2 b)
{
	vec2 complexMultiply;
	complexMultiply.x = a.x*b.x - a.y*b.y;
	complexMultiply.x = a.x*b.y + a.y*b.x;
	return complexMultiply;
}

vec2 conjugate(vec2 z) {
	return vec2(z.x,-z.y);
}

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// dispersion calculator
//w = sqrt(gk);

float dispersion(int mprime, int nprime)
{
	float w_0 = 2.0f * M_PI / 200.0f; // normalizing factor
	float kx = M_PI * (2 * mprime - vars.N)/vars.length;
	float ky = M_PI * (2 * nprime - vars.N)/vars.length;
	float c = sqrt(GRAV* sqrt(kx*kx+ ky*ky));
	return floor(c/w_0)*w_0;
}

float phillips(int mprime, int nprime )
{
	float kx = M_PI * (2 * mprime - vars.N)/vars.length;
	float ky = M_PI * (2 * nprime - vars.N)/vars.length;
	vec2 k = vec2(kx,ky);
	vec2 kdir= normalize(k);
	float kDotw = dot( kdir,vars.windDir);
	float kDotwSquared = kDotw*kDotw;
	float kLength = length(k);
	if (kLength < 0.000001) 
		return 0.0;

	float kLFourth = kLength*kLength*kLength*kLength;
	
	float lengthSquared = length(vars.windDir);
	float L = lengthSquared*lengthSquared/GRAV;
	float damping   = 0.0001;
	 float l2 =L * damping * damping;

	float intm = vars.A*exp(-1*(kLength*kLength*l2));
	float ph = intm/kLFourth;

	
	return ph*exp(-kLength*kLength*l2);
}

vec2 ho( int mprime, int nprime)
{
	vec2 gaussrand;
	gaussrand.x= rand(vec2(mprime,nprime));
	gaussrand.y= rand(vec2(nprime,mprime));
	return gaussrand * sqrt(phillips(mprime, nprime) / 2.0f);
}
vec2 h(int mprime, int nprime,float time)
{
	float omegat = dispersion(mprime, nprime) * time;

	float real = cos(omegat);
	float imag = sin(omegat);
	vec2 c = vec2(real,imag);
	vec2 cprime = vec2(real,-imag);
	vec2 a = ho(mprime,nprime);
	vec2 b = ho(-mprime,-nprime);
	b = conjugate(b);
	vec2 hoprime;
	hoprime= complexMulitply(a,c)+ complexMulitply(b,cprime);
	return hoprime;
}



void main() {
 uint  gid = gl_GlobalInvocationID.y*32*gl_NumWorkGroups.x+gl_GlobalInvocationID.x;	
 vec2 h = h(int(gl_GlobalInvocationID.x),int(gl_GlobalInvocationID.y),currentTime);
 F[gid] = vec4(h.x,h.y,0.0,0.0);  
}