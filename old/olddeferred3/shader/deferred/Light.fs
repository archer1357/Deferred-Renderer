
layout(binding = 0) uniform sampler2D u_colMap;
layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;

varying vec2 v_tex;

uniform mat4 u_invProjMat;
uniform vec3 u_lightCol;
uniform float u_strength;

#if defined POINT || defined SPOT
uniform vec4 u_lightPos;
uniform vec3 u_lightAtten;
#endif

#ifdef SPOT
uniform float u_spotCosCutoff;
uniform float u_spotExponent;
#endif

#if defined SPOT || defined DIRECTIONAL
uniform vec4 u_lightDir;
#endif

void main() {
  float depth = texture2D(u_depthMap, v_tex).x;

  if(depth==1.0) {
    discard;
  }

  vec4 col2=texture2D(u_colMap,v_tex);
  vec4 nor2=texture2D(u_norMap,v_tex);
  float shininess=0.0;//col2.a;

  vec3 col=col2.rgb;
  vec3 nor=nor2.xyz;

  if(nor==vec3(0.0)) {
    discard;
  }

  //
  nor=normalize(nor);
  vec4 H = vec4(vec3(v_tex,depth)*2.0-1.0, 1.0);
  vec4 D = u_invProjMat*H;
  vec3 pos = (D / D.w).xyz;

#if defined POINT || defined SPOT
  vec3 lightDir=u_lightPos.xyz-pos;
  float lightDist=length(lightDir);
  lightDir=lightDir/lightDist;

  float atten = 1.0/(u_lightAtten.x+u_lightAtten.y*lightDist+
                     u_lightAtten.z*lightDist*lightDist);
#endif

#ifdef SPOT
  vec3 spotDir=u_lightDir.xyz;
  float spotCos = dot(lightDir, -spotDir);

  if(spotCos < u_spotCosCutoff) {
    discard;
    atten = 0.0;
  } else {
    atten *= pow(spotCos, u_spotExponent);
  }
#endif

#ifdef DIRECTIONAL
  vec3 lightDir=normalize(-u_lightDir.xyz);
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
  vec3 reflectedLight = u_lightCol *spec;

#if defined POINT || defined SPOT
  scatteredLight *= atten;
  reflectedLight *= atten;
#endif

  vec3 rgb = col.rgb*(scatteredLight + reflectedLight);

  gl_FragColor=vec4(rgb,1.0);
  
  //gl_FragColor=vec4(scatteredLight,1.0);
  //gl_FragColor=((u_lightDir+vec4(1.0))/2.0);
  //gl_FragColor=vec4((nor+vec3(1.0))/2.0,1.0);
}
