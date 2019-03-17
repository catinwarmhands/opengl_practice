#version 330 core

uniform vec3 iResolution;
uniform float iGlobalTime;
uniform vec2 iMouse;


// #define SHOWONLYEDGES
//#define NYAN 
#define WAVES
// #define BORDER

#define RAY_STEPS 150

#define BRIGHTNESS 1.2
#define GAMMA 1.4
#define SATURATION .65


#define detail .001
#define t iGlobalTime*.3

void main() {
	float time = iGlobalTime*3;
	vec2 uv  = (gl_FragCoord.xy / 400.0-0.5)*8.0;
	vec2 uv0 = uv;
	float i0 = 1.0;
	float i1 = 1.0;
	float i2 = 1.0;
	float i4 = 0.0;
	for(int s = 0; s < 7; s++) {
		vec2 r = vec2(cos(uv.y*i0-i4+time/i1), sin(uv.x*i0-i4+time/i1))/i2;
		r += vec2(-r.y,r.x)*0.3;
		uv.xy += r;

		i0 *= 1.93;
		i1 *= 1.15;
		i2 *= 1.7;
		i4 += 0.05+0.1*time*i1;
	}
	float r = sin(uv.x-time)*0.5+0.5;
	float b = sin(uv.y+time)*0.5+0.5;
	float g = sin((uv.x+uv.y+sin(time*0.5))*0.5)*0.5+0.5;
	r *= 2;
	gl_FragColor = vec4(r,g,b,1);
}
