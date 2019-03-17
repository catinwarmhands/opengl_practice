#version 330 core

uniform ivec2 iResolution;
uniform float iGlobalTime;
uniform vec2 iMouse;

void main() {
	// gl_FragColor = vec4(0.1, 0.1, 0.9, 1.);
	gl_FragColor = vec4(abs(cos(iGlobalTime*2)), abs(sin(iGlobalTime)), 1, 1.);
}
