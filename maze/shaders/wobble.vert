#version 330 core

layout (location = 0) in vec3 position;
uniform ivec2 iResolution;
uniform float iGlobalTime;
uniform vec2 iMouse;

void main() {
	int i = gl_VertexID+1;
	vec4 result = vec4(position.xyz, 1);
	result.xy += vec2(sin(iGlobalTime*(3+i))/15, sin(iGlobalTime*(4+i))/15);
	gl_Position = result;
}