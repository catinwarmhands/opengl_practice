#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
uniform float time;

// out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	vec3 pos = position;
	if (pos.y > 0.3f) {
		pos.x += cos(time)/15.0f;
		pos.z += sin(time/2.0f)/15.0f;
	}
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	// gl_Position.x *= sin(time);
    TexCoord = texCoord;
}