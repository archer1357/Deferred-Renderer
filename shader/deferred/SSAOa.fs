#version 140

#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;


//uniform float u_ambience;


in vec2 v_tex;
 




uniform float u_zNear;
uniform float u_zFar;

uniform vec2 u_screenSize;

layout(location = 0) out vec4 fragColor;

 
float readDepth( in vec2 coord ) {
	return (2.0 * u_zNear) / (u_zFar + u_zNear - texture2D( u_depthMap, coord ).x * (u_zFar - u_zNear));	
}
 
float compareDepths( in float depth1, in float depth2 ) {
	float aoCap = 1.0;
	float aoMultiplier=10000.0;
	float depthTolerance=0.000;
	float aorange = 100.0;//10.0;// units in space the AO effect extends to (this gets divided by the camera far range
	float diff = sqrt( clamp(1.0-(depth1-depth2) / (aorange/(u_zFar-u_zNear)),0.0,1.0) );
	float ao = min(aoCap,max(0.0,depth1-depth2-depthTolerance) * aoMultiplier) * diff;
	return ao;
}
 vec2 rand(in vec2 coord) //generating random noise
{
float noiseX = (fract(sin(dot(coord ,vec2(12.9898,78.233))) * 43758.5453));
float noiseY = (fract(sin(dot(coord ,vec2(12.9898,78.233)*2.0)) * 43758.5453));
return vec2(noiseX,noiseY)*0.0003;
}
void main(void)
{	

  if(texture2D(u_depthMap, v_tex).x==1.0) {
    discard;
  }

  vec4 nor2=texture2D(u_norMap,v_tex);
  vec3 nor=nor2.xyz;
  float emissive=nor2.a;
  
  if(emissive != 0.0) {
    //discard;
  }
  
  //if(nor == vec3(0.0,0.0,0.0)) {
  //  discard;
  //}

	float depth = readDepth( v_tex );
	float d;
 
	float pw = 1.0 / u_screenSize.x;
	float ph = 1.0 / u_screenSize.y;
 
	float aoCap = 1.0;
 
	float ao = 0.0;
 
	float aoMultiplier=10000.0;
 
	float depthTolerance = 0.001;
 
	float aoscale=1.0;
 
 for(int i=0;i<4;i++) {
   
if(i!=0) {
	pw*=2.0;
	ph*=2.0;
	aoMultiplier/=2.0;
	aoscale*=1.2;
  }
  
       float npw = (pw);
       float nph = (ph);

	d=readDepth( vec2(v_tex.x+npw,v_tex.y+nph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-npw,v_tex.y+nph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x+npw,v_tex.y-nph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-npw,v_tex.y-nph));
	ao+=compareDepths(depth,d)/aoscale;

 }
 
	/*

	d=readDepth( vec2(v_tex.x+pw,v_tex.y+ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-pw,v_tex.y+ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x+pw,v_tex.y-ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-pw,v_tex.y-ph));
	ao+=compareDepths(depth,d)/aoscale;
  
  pw*=2.0;
	ph*=2.0;
	aoMultiplier/=2.0;
	aoscale*=1.2;
 
	d=readDepth( vec2(v_tex.x+pw,v_tex.y+ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-pw,v_tex.y+ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x+pw,v_tex.y-ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-pw,v_tex.y-ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	pw*=2.0;
	ph*=2.0;
	aoMultiplier/=2.0;
	aoscale*=1.2;
 
	d=readDepth( vec2(v_tex.x+pw,v_tex.y+ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-pw,v_tex.y+ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x+pw,v_tex.y-ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-pw,v_tex.y-ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	pw*=2.0;
	ph*=2.0;
	aoMultiplier/=2.0;
	aoscale*=1.2;
 
	d=readDepth( vec2(v_tex.x+pw,v_tex.y+ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-pw,v_tex.y+ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x+pw,v_tex.y-ph));
	ao+=compareDepths(depth,d)/aoscale;
 
	d=readDepth( vec2(v_tex.x-pw,v_tex.y-ph));
	ao+=compareDepths(depth,d)/aoscale;
  */
 
	ao/=16.0;
  //ao=clamp(ao,0.35,0.6);
	fragColor = vec4(vec3(1.0-ao),1.0);
}