#version 330 core

layout (location = 0) in vec3 position;
uniform ivec2 iResolution;
uniform float iGlobalTime;
uniform vec2 iMouse;

void main() {
	gl_Position = vec4(position.xyz, 1);
}