#version 330 
#define M_PI 3.1415926535897932384626433832795
#define GRAV 9.81
precision highp float;

in vec3 inPosition;
in vec2 vertTexCoord;
out vec3 vertexPos;

uniform float currentTime;
uniform mat4 MVP;
out vec2 fragTexCoord;
out vec2 UV;
out vec2 debug;

struct compositeReturn
{
	float height;
	vec2 offset;
};

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
	float L = lengthSquared*lengthSquared/9.81;
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

compositeReturn height(vec2 position, float time)
{
	vec2 heightReal = vec2(0,0);
	vec2 displacement = vec2(0,0);
	for(int i =0;i<vars.N;i++)
	{
		for(int j =0;j<vars.N;j++)
		{
			if(i == vars.N/2 || j == vars.N/2)
				continue;

			float kx =position.x* M_PI * (2 * i - vars.N)/vars.length;
			float ky = position.y*M_PI * (2 * j - vars.N)/vars.length;
			vec2 k = vec2(kx,ky);
			float klength = length(k);
			 
			float cosComponent = cos(kx+ky);
			float sinComponent = sin(kx+ky);
			vec2 c = vec2(cosComponent,	sinComponent);		
			vec2 htilde  =  h(i, j,time);
			heightReal +=complexMulitply(htilde,c);
			// calculate displacement
		
			displacement += vec2(kx/klength *heightReal.y/200000,ky/klength*heightReal.y/200000);
		}
	}
	compositeReturn compRet;
	compRet.height = sqrt(pow(heightReal.x,2)+pow(heightReal.y,2))/15.0;
	compRet.offset= displacement;

	return compRet;
}

void main()
{
	UV = inPosition.xy;
    // hard code the mvp right now
	vec3 position = inPosition.xyz;

	debug = position.xz;
	gl_Position = vec4(position.xyz,1);
}