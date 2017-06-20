#version 140

#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

//High-performance FXAA shader, adapted from:
//https://code.google.com/p/processing/source/browse/trunk/processing/java/libraries/opengl/examples/Shaders/FXAA/data/fxaa.glsl?r=9668

// Shader Settings
// The parameters are hardcoded for now, but could be
// made into uniforms to control from the program.

//The maximum span to search along an edge to find and smooth jaggies 
//Increasing this wont do much unless you also reduce FXAA_REDUCE_MUL (e.g. increase the denominator)
//Increase this too much, and the quality may suffer (I recommend the default).
float FXAA_SPAN_MAX = 8.0; 

//The overall strength of the smoothing effect. A smaller value (larger denominator) is stronger (1.0/8.0 is good for most purposes).
//Increase the strength (e.g increase denominator) too much and the quality may suffer.
float FXAA_REDUCE_MUL = 1.0/18.0;

//Effects the threshold for applying the smoothing effect (threshold is based on local contrast).
//A larger value (smaller denominator) increases the threshold, thus preserving detail better (but leaving more jaggies behind).
float FXAA_REDUCE_MIN = (1.0/18.0);

//Texture detail preservation factor. A higher value will protect texture detail more. A lower value will smooth textures more.
float FXAA_DETAIL_PRESERVE = 4.0;

//If you decrease FXAA_DETAIL_PRESERVE to less than 3.0, I recommend you also try decreasing the strength (decreasing the 
//denominators) of FXAA_REDUCE_MUL and FXAA_REDUCE_MIN. You simply dont need as much smoothing when you turn down the texture 
//detail preservation, and a combination of low detail preservation and high smoothing can cause weird dot artifacts in textures.




layout(binding = 0) uniform sampler2D bgl_RenderedTexture;

uniform vec2 u_screenSize;

float bgl_RenderedTextureWidth=u_screenSize.x;
float bgl_RenderedTextureHeight=u_screenSize.y;

// The inverse of the texture dimensions along X and Y
vec2 texcoordOffset = vec2(1.0/bgl_RenderedTextureWidth, 1.0/bgl_RenderedTextureHeight);

in vec2 v_tex;

vec2 vertTexcoord = v_tex;

layout(location = 0) out vec4 fragColor;

void main() {  

  vec3 rgbNW = texture2D(bgl_RenderedTexture, vertTexcoord.xy + (vec2(-1.0, -1.0) * texcoordOffset)).xyz;
  vec3 rgbNE = texture2D(bgl_RenderedTexture, vertTexcoord.xy + (vec2(+1.0, -1.0) * texcoordOffset)).xyz;
  vec3 rgbSW = texture2D(bgl_RenderedTexture, vertTexcoord.xy + (vec2(-1.0, +1.0) * texcoordOffset)).xyz;
  vec3 rgbSE = texture2D(bgl_RenderedTexture, vertTexcoord.xy + (vec2(+1.0, +1.0) * texcoordOffset)).xyz;
  vec3 rgbM  = texture2D(bgl_RenderedTexture, vertTexcoord.xy).xyz;
	
  vec3 luma = vec3(0.299, 0.587, 0.114);
  float lumaNW = dot(rgbNW, luma);
  float lumaNE = dot(rgbNE, luma);
  float lumaSW = dot(rgbSW, luma);
  float lumaSE = dot(rgbSE, luma);
  float lumaM  = dot( rgbM, luma);
	
  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
  
  float lumaRange = abs(lumaMax - lumaMin);
	
  vec2 dir;
  dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
  dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
  
  float texFac = min((1.25*FXAA_DETAIL_PRESERVE)*min(abs(dir.x), abs(dir.y)), 1.0);
	
  float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	  
  float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	
  dir = min(vec2(FXAA_SPAN_MAX,  FXAA_SPAN_MAX), 
        max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * texcoordOffset;
		
  vec3 rgbA = (1.0/2.0) * (
              texture2D(bgl_RenderedTexture, vertTexcoord.xy + dir * (1.0/3.0 - 0.5)).xyz +
              texture2D(bgl_RenderedTexture, vertTexcoord.xy + dir * (2.0/3.0 - 0.5)).xyz);
  float lumaA = dot(rgbA, luma);     
  
  vec3 Bsample1 = texture2D(bgl_RenderedTexture, vertTexcoord.xy + dir * (0.0/3.0 - 0.5)).xyz;
  float Bluma1 = dot(Bsample1, luma);
  vec3 Bsample2 = texture2D(bgl_RenderedTexture, vertTexcoord.xy + dir * (3.0/3.0 - 0.5)).xyz;
  float Bluma2 = dot(Bsample2, luma);
  
  vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (Bsample1+Bsample2);
  float lumaB = dot(rgbB, luma);
  
  float Xsign = lumaA > Bluma1 ? 1.0:-1.0;
  
  float peakFac =clamp( FXAA_DETAIL_PRESERVE*  max(  min(Xsign*(lumaA - Bluma1), Xsign*(lumaA - Bluma2))  , 0.0 )   /  (lumaRange+.0001) -.25    , 0.0, 1.0);
  
  vec3 color;
  if((lumaB < lumaMin) || (lumaB > lumaMax)){
    color = rgbA;
  }else { 
    color = mix(rgbB, rgbA, peakFac);
  }
  fragColor.xyz=mix(color, rgbM, texFac);
  fragColor.a = 1.0;
  
  //gl_FragColor.xyz = vec3(texFac);  
  
}