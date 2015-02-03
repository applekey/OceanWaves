#version 330
precision highp float;
in vec2 UV;
in float debug;

void main() {
	//gl_FragColor = vec4(debug);
	gl_FragColor = vec4(0.5);
}
