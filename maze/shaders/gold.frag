#version 330 core
// in vec3 ourColor;
uniform float time;

in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

void main()
{

  vec2 uv = TexCoord;
  vec2 uv2 = uv;
  uv.y += time / 10.0;
  uv.x -= (sin(time/10.0)/2.0);
  
  uv2.y += time / 14.0;
  uv2.x += (sin(time/10.0)/9.0);
  float result = 0.0;
  result += texture(ourTexture2, uv * 0.6 + vec2(time*-0.003)).r;
  result *= texture(ourTexture2, uv2 * 0.9 + vec2(time*+0.002)).b;
  result = pow(result, 3.0);
  // fragColor = vec4(180.0)*result;
  color = texture(ourTexture1, TexCoord) + result*20;
}
