#version 330 

precision highp float;

in vec4 position;
in vec2 texCoord;
uniform mat4 MVP;

uniform sampler2D data;

out vec3 normal;
out vec3 vertexPos;

float stepW = 1.0/128;
float stepH = 1.0/128;

vec3 calcnormal()
{
		 vec2 current = position.xz;
	      /*

		  a  b  c
		  d  e  f
		  g  h  i

		  */
		
		  vec3 e;
		  e.xy = vec2(0.0,0.0);
		  e.z = texture(data, texCoord).r;

		  vec3 f;
		  f.xy = vec2(stepW, 0.0);
		  f.z = texture(data, current + vec2(stepW, 0.0)).r;
		  
		  vec3 i;
		  i.xy = vec2(stepW, stepH);
		  i.z= texture(data, current + vec2(stepW, stepH)).r;  //i

		  vec3 h; 
		  h.xy = vec2(0.0, stepH);
		  h.z= texture(data, current + vec2(0.0, stepH)).r;  //h

		  vec3 g;
		  g.xy = vec2(-stepW, stepH);
		  g.z = texture(data, current + vec2(-stepW, stepH)).r; //g

		  vec3 d;
		  d.xy = vec2(-stepW, 0.0);
		  d.z = texture(data, current + vec2(-stepW, 0.0)).r; //d
		  
		  vec3 a;
		  a.xy = vec2(-stepW, -stepH);
		  a.z = texture(data, current + vec2(-stepW, -stepH)).r; //a

		  vec3 b;
		  b.xy =  vec2(0.0, -stepH);
		  b.z = texture(data, current + vec2(0.0, -stepH)).r; //b
		  
		  vec3 c;
		  c.xy = vec2(stepW, -stepH);
		  c.z = texture(data, current + vec2(stepW, -stepH)).r; //c
    	 
		  vec3 fe = (f- e);
		  vec3 ie = (i -e);
		  vec3 he = (h -e);
		  vec3 ge = (g -e);
		  vec3 de = (d -e);
		  vec3 ae = (a -e);
		  vec3 be = (b -e);
		  vec3 ce = (c -e);

		  vec3 normal = vec3(0.0);
		  normal += cross(fe,ie);
		  normal += cross(ie,he);
		  normal += cross(he,ge);
		  normal += cross(ge,de);
		  normal += cross(de,ae);
		  normal += cross(ae,be);
		  normal += cross(be,ce);
		  
		  // normalize the normal
		  normal = normalize(normal);
		  return normal;

}

void main()
{
	vec3 newPosition = position.xyz;
    float z = texture2D(data,position.xz).r;
	
	// calculate the normal
	normal = calcnormal();

	newPosition.y = z;
	vertexPos = newPosition.xyz;
	gl_Position = MVP*vec4(newPosition.xyz,1);


}