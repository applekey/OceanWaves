#version 430
#extension GL_ARB_shader_storage_buffer_object : enable


precision highp float;

layout( std140, binding=4 ) buffer Buff {
	vec4 F[ ];  //write buffer
};

layout( std140, binding=4 ) buffer Twittle {
	vec4 T[ ];  //write buffer
};

layout (local_size_x = 128, local_size_y = 1,local_size_z = 1) in; // 32 fft
//////////////////////////////////////////////////////////////////////////////

void main() {
 uint  gid = gl_GlobalInvocationID.x;	
 int iterationCount = 8;
 // we will need 8 iterations
 for(int i =0;i<8;i++)
{

}



}