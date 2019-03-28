#version 330 core
// in vec3 ourColor;
uniform float time;

in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{
	// float ndcDepth = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);
	// float clipDepth = ndcDepth / gl_FragCoord.w;
	// vec4 dcolor = vec4((clipDepth * 0.01) + 0.5); 
	// color = vec4(sin(time), 0.3, 0.3, 1);
    color = texture(ourTexture1, TexCoord);
    // color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.5);
}