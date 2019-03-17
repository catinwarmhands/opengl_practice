#version 330 core

uniform vec3 iResolution;
uniform float iGlobalTime;
uniform vec2 iMouse;

void main() {
	gl_FragColor = vec4(0.1, 0.1, 0.9, 1.);

	// gl_FragColor = vec4(abs(cos(iGlobalTime*8)), abs(sin(iGlobalTime)), 1, 1.);

	// float intensity = 1.0 / length(gl_FragCoord.xy - iMouse);
	// gl_FragColor = vec4(1,0,0,0);
	// gl_FragColor *= intensity * 20;
}
