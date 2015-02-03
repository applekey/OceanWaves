#version 330
precision highp float;
in vec3 normal;
in vec3 vertexPos;

out vec3 color;

void main() {
		// light material properties
		vec4 lightPosition = vec4(0.0f, 5.0f, 10.0f, 100.0f);
		vec4 lightAmbient = vec4(0.0f, 0.0f, 0.9f, 1.0f);
		vec4 lightDifuse = vec4(0.0588f, 0.0705f, 0.792f, 1.0f);
		vec4 lightSpec = vec4(0.0f, 1.0f, 1.0f, 0.8f);

		// set up paramters
		vec3 L = normalize(lightPosition.xyz - vertexPos);   
		vec3 E = normalize(-vertexPos); // we are in Eye Coordinates, so EyePos is (0,0,0)  
		vec3 R = normalize(-reflect(L,normal));  
 
		//calculate Ambient Term:  
		vec4 Iamb = lightAmbient;    

		//calculate Diffuse Term:  
		vec4 Idiff = lightDifuse * max(dot(normal,L), 0.0);
		Idiff = clamp(Idiff, 0.0, 1.0);     
   
		// material properties
		float shiny =  3.0f;

		// calculate Specular Term:
		vec4 Ispec = lightSpec 
				* pow(max(dot(R,E),0.0),0.3*shiny);
		Ispec = clamp(Ispec, 0.0, 1.0); 
		// write Total Color:  

		//color = texture2D( normalImage, UV ).rgb;
		color = vec3(0.5,0.5,0.5) + (Iamb.xyz/2.0 + Idiff.xyz + Ispec.xyz)/3.0;
}
