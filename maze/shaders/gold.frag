#version 330 core
// in vec3 ourColor;
uniform float time;

in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

// defining Blending functions
#define Blend(base, blend, funcf) 		vec4(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b), funcf(base.a, blend.a))
#define BlendAddthird(base, blend) 		min(base + (blend*0.3), vec4(1.0))
#define BlendAddtenth(base, blend) 		min(base + (blend*0.06), vec4(1.0))


// distance calculation between two points on the Y-plane
float dist(vec2 p0, vec2 pf){
     return sqrt((pf.y-p0.y)*(pf.y-p0.y));
}

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r) {
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
{ 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

	// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

	// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

	// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

	// Gradients: 7x7 points over a square, mapped onto an octahedron.
	// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

	//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

	// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

vec3 hsv(float h, float s, float v)
{
  return mix( vec3( 1.0 ), clamp( ( abs( fract(
    h + vec3( 3.0, 2.0, 1.0 ) / 3.0 ) * 6.0 - 3.0 ) - 1.0 ), 0.0, 1.0 ), s ) * v;
}

void main()
{
	// color = vec4(sin(time), 0.3, 0.3, 1);
 //    vec4 sandcolor = vec4(0.9606, 0.6601, 0.1445, 1.0);
	// color = texture(ourTexture1, TexCoord)*sandcolor;

	vec2 uv = TexCoord.xy;
    float fadeLR = .5 - abs(uv.x - .5) ;
    float fadeTB = 1.-uv.y;
    vec3 pos = vec3( uv * vec2( 3. , 1.) - vec2( 0. , time * .03 ) , time * .01 );
    float n = fadeLR * fadeTB * smoothstep(.6, 1. ,snoise( pos * 60. )) * 10.;
    vec3 col = hsv( n * .2 + .7 , .4 , 1. );
	color =  vec4( col * vec3( n ) ,n);

	// vec2 coord = TexCoord;
 //    vec4 sandtexture = texture(ourTexture1, coord);
 //    vec4 sandspecular = texture(ourTexture1, coord);
 //    vec2 plusuv = floor(coord -sin(time*10));
	// vec2 reverseuv = floor(coord +cos(time*10));
 //    vec4 sandspecular2 = texture(ourTexture1, reverseuv);
 //    vec4 sandspecular3 = texture(ourTexture1, plusuv);
	// sandspecular.xyz = sandspecular.xxx*sandspecular3.yyy*sandspecular2.zzz*vec3(2,2,2);
 //    float d = abs(coord.y - ((1.3 + sin(time*5))*200.0)); //for mouse input: abs(fragCoord.y - iMouse.y)
 //    // d = d*0.003;
 //    d = pow(d,0.6);
 //    d = min(d,1.0);
 //    vec4 sandbase = BlendAddtenth(sandcolor,sandtexture);
 //  	vec4 darkensand = mix(sandtexture,vec4(0,0,0,0), d);
 //    vec4 gradientgen = mix(sandspecular, darkensand, d);
 //    vec4 finalmix = BlendAddthird(sandbase, gradientgen);
 //    color = finalmix;

 //    vec4 sandcolor = vec4(0.9606, 0.6601, 0.1445, 1.0);
 //    vec4 sandtexture = texture(ourTexture1, gl_FragCoord   / iResolution.xy);
 //    vec4 sandspecular = texture(ourTexture1, gl_FragCoord   / iResolution.xy);
 //    vec2 plusuv = floor(gl_FragCoord -sin(iMouse.yy*0.03));
	// vec2 reverseuv = floor(gl_FragCoord +cos(iMouse.yy*0.018));
 //    vec4 sandspecular2 = texture(ourTexture1, reverseuv  / iResolution.xy);
 //    vec4 sandspecular3 = texture(ourTexture1, plusuv  / iResolution.xy);
	// sandspecular.xyz = sandspecular.xxx*sandspecular3.yyy*sandspecular2.zzz*vec3(2,2,2);
 //    float d = abs(gl_FragCoord .y - ((1.3 + sin(iTime))*200.0)); //for mouse input: abs(fragCoord.y - iMouse.y)
 //    d = d*0.003;
 //    d = pow(d,0.6);
 //    d = min(d,1.0);
 //    vec4 sandbase = BlendAddtenth(sandcolor,sandtexture);
 //  	vec4 darkensand = mix(sandtexture,vec4(0,0,0,0), d);
 //    vec4 gradientgen = mix(sandspecular, darkensand, d);
 //    vec4 finalmix = BlendAddthird(sandbase, gradientgen);
 //    color = finalmix;
}

///////////////////


