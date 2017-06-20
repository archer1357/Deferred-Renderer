#version 140

#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(binding = 5) uniform sampler2D rnm;
layout(binding = 1) uniform sampler2D normalMap;
layout(location = 0) out vec4 fragColor;

in vec2 v_tex;

const float totStrength = 1.38;
const float strength = 0.07;
const float offset = 18.0;
const float falloff = 0.000002;

const float rad = 0.006;

#define SAMPLES 10 // 10 is good
const float invSamples = -1.38/10.0;

// NOTE: THIS ONE IS BRUTALLY OPTIMIZED!! SO IT*S REALLY HARD TO FOLLOW

void main(void)
{

// these are the random vectors inside a unit sphere
vec3 pSphere[10] = vec3[](vec3(-0.010735935, 0.01647018, 0.0062425877),vec3(-0.06533369, 0.3647007, -0.13746321),vec3(-0.6539235, -0.016726388, -0.53000957),vec3(0.40958285, 0.0052428036, -0.5591124),vec3(-0.1465366, 0.09899267, 0.15571679),vec3(-0.44122112, -0.5458797, 0.04912532),vec3(0.03755566, -0.10961345, -0.33040273),vec3(0.019100213, 0.29652783, 0.066237666),vec3(0.8765323, 0.011236004, 0.28265962),vec3(0.29264435, -0.40794238, 0.15964167));
   
   // grab a normal for reflecting the sample rays later on
   vec3 fres = normalize((texture2D(rnm,v_tex*offset).xyz*2.0) - vec3(1.0));
   
   vec4 currentPixelSample = texture2D(normalMap,v_tex);
   
   float currentPixelDepth = currentPixelSample.a;
   
   // current fragment coords in screen space
   vec3 ep = vec3(v_tex.xy,currentPixelDepth);

   // get the normal of current fragment
   vec3 norm = currentPixelSample.xyz;
   
   float bl = 0.0;

   // adjust for the depth ( not shure if this is good..)
   float radD = rad/currentPixelDepth;
   
   //vec3 ray, se, occNorm;
   float occluderDepth, depthDifference;
   vec4 occluderFragment;
   vec3 ray;

   for(int i=0; i<SAMPLES;++i)
   {
      // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
      ray = radD*reflect(pSphere[i],fres);
      
      // if the ray is outside the hemisphere then change direction
      //se = ep + sign(dot(ray,norm) )*radD*reflect(pSphere[i],fres).xy;
      	
      // get the depth of the occluder fragment
      occluderFragment = texture2D(normalMap,ep.xy + sign(dot(ray,norm) )*ray.xy);
      
      // get the normal of the occluder fragment
      //occNorm = occluderFragment.xyz;
      
      // if depthDifference is negative = occluder is behind current fragment
      depthDifference = currentPixelDepth-occluderFragment.a;
      
      // calculate the difference between the normals as a weight

      //normDiff = (1.0-dot(occluderFragment.xyz,norm)); // used to be 1.0 - 
      
	// the falloff equation, starts at falloff and is kind of 1/x^2 falling 
      bl += step(falloff,depthDifference)*(1.0-dot(occluderFragment.xyz,norm))*(1.0-smoothstep(falloff,strength,depthDifference));
   }
   
   // output the result
  // float ao = 1.0-totStrength*bl*invSamples;
    //fragColor = vec4(vec3(1.0+bl*invSamples),1.0);
    float ao=1.0+bl*invSamples;
	//fragColor = vec4(vec3(1.0-ao),1.0);
 	fragColor=vec4(vec3(ao),1.0);
   //gl_FragColor.xyz = norm;
   
}