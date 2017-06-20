#ifdef GL_ES
uniform sampler2D u_colMap;
uniform sampler2D u_norMap;
uniform sampler2D u_depthMap;
#else
layout(binding = 0) uniform sampler2D u_colMap;
layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;
#endif

// vec3 unpackColor(float f) {
//     vec3 color;
//     color.b = floor(f / 256.0 / 256.0);
//     color.g = floor((f - color.b * 256.0 * 256.0) / 256.0);
//     color.r = floor(f - color.b * 256.0 * 256.0 - color.g * 256.0);
//     // now we have a vec3 with the 3 components in range [0..256]. Let's normalize it!
//     return color / 256.0;
// }

uniform mat4 u_invProjMat;
uniform float u_strength;

varying vec2 v_tex;

#if defined POINT || defined SPOT
uniform vec3 u_lightPos;
uniform vec3 u_lightAtten;
#endif

#ifdef DIRECTIONAL
uniform vec3 u_lightDir;
#endif

uniform vec3 u_lightCol;

#ifdef SPOT
uniform vec3 u_spotDir;
uniform float u_spotCosCutoff;
uniform float u_spotExponent;

#endif


void main() {
  float depth = texture2D(u_depthMap, v_tex).x;
  if(depth==1.0) {discard;}

  vec4 col=texture2D(u_colMap,v_tex);
  vec4 nor2=texture2D(u_norMap,v_tex);
  float shininess=0.7;//col.a;
  if(nor2.a <-2.0) {
// if(col.a <0.0) {
// col.rgb=vec3(0.0,0.0,1.0);
  }
  vec3 nor=nor2.xyz;
  // col.rgb=unpackColor(texture2D(u_norMap,v_tex).a);
  // col.rgb=vec3(1.0,1.0,1.0);
  if(nor==vec3(0.0)) {
    discard;
  }

  nor=normalize(nor);
  vec4 H = vec4(vec3(v_tex,depth)*2.0-1.0, 1.0);
  vec4 D =  u_invProjMat *H;
  vec3 pos = (D / D.w).xyz;

#if defined POINT || defined SPOT
  vec3 lightDir=u_lightPos-pos;
  float lightDist=length(lightDir);
  lightDir=lightDir/lightDist;

  float atten = 1.0/(u_lightAtten.x+u_lightAtten.y*lightDist+
                     u_lightAtten.z*lightDist*lightDist);
#endif

#ifdef SPOT
  float spotCos = dot(lightDir, -u_spotDir);

  if(spotCos < u_spotCosCutoff) {
    discard;
    atten = 0.0;
  } else {
    atten *= pow(spotCos, u_spotExponent);
  }
#endif

#ifdef DIRECTIONAL
  vec3 lightDir=normalize(-u_lightDir);
#endif

  vec3 eyeDir=normalize(-pos);
  vec3 halfVector = normalize(lightDir + eyeDir);

  float NdotL = max(0.0, dot(nor, lightDir));
  float spec = 0.0;

  if(NdotL > 0.0) {
    float NdotHV = max(0.0, dot(nor, halfVector));
    spec = pow(NdotHV, shininess) * u_strength;
  }

  vec3 scatteredLight = u_lightCol * NdotL;
  // vec3 reflectedLight = vec3(spec);
  vec3 reflectedLight = u_lightCol *spec;
#if defined POINT || defined SPOT
  scatteredLight *= atten;
  reflectedLight *= atten;
#endif
// col.rgb=vec3(1.0,1.0,1.0);
  //vec3 rgb = min(scatteredLight + reflectedLight,vec3(1.0));
  vec3 rgb = col.rgb*scatteredLight ;//+ reflectedLight;
  // rgb=abs(nor);


  gl_FragColor=vec4(rgb,1.0);
}
