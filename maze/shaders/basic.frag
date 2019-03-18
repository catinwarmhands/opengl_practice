#version 330 core

out vec4 color;

void main() {
	color = vec4(0.1, 0.1, 0.9, 1.);
	// gl_FragColor = vec4(abs(cos(iGlobalTime*2)), abs(sin(iGlobalTime)), 1, 1.);
}
