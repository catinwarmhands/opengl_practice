#version 330 core
// in vec3 ourColor;
uniform float time;

in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

uniform vec2 cursorPosition;
uniform vec2 resolution;


#define M_PI 3.1415926535897932384626433832795

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


void main()
{
  vec2 coord = gl_FragCoord.xy;
  coord.x += cursorPosition.x;
  coord.y -= cursorPosition.y;
  float size = 30.0;
  float prob = 0.95;
  
  vec2 pos = floor(1.0 / size * coord);
  
  float result = 0.0;
  float starValue = rand(pos);
  
  if (starValue > prob)
  {
    vec2 center = size * pos + vec2(size, size) * 0.5;
    
    float t = 0.9 + 0.2 * sin(time + (starValue - prob) / (1.0 - prob) * 45.0);
        
    result = 1.0 - distance(coord, center) / (0.5 * size);
    result = result * t / (abs(coord.y - center.y)) * t / (abs(coord.x - center.x));
  }
  else if (rand(coord / resolution.xy) > 0.996)
  {
    float r = rand(coord);
    result = r * (0.25 * sin(time * (r * 5.0) + 720.0 * r) + 0.75);
  }
  
  color = vec4(vec3(result), 1.0);
}
