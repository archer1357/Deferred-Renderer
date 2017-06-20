
#define COLOR

#ifdef COLOR
layout(binding = 0) uniform sampler2D u_colMap;
#endif


#if defined NORMAL || defined RELIEF2
layout(binding = 1) uniform sampler2D u_norMap;
#endif

#if defined BUMP || defined RELIEF2
layout(binding = 2) uniform sampler2D u_hgtMap;
#endif

#ifdef BUMP
uniform float u_bumpScale;
uniform float u_bumpBias;
#endif

uniform float u_reflective;
uniform float u_shininess;

#if defined BUMP || defined RELIEF
varying vec3 v_eyeDir;
#endif

varying vec2 v_tex;
varying vec3 v_nor;

#if defined BUMP || defined RELIEF || defined RELIEF2
varying vec3 v_tan;
varying vec3 v_bin;
#endif

void main() {

#if defined BUMP || defined RELIEF || defined RELIEF2
  vec3 t=normalize(v_tan);
  vec3 b=normalize(v_bin);
#endif

  vec3 n=normalize(v_nor);

#if defined BUMP || defined RELIEF || defined RELIEF2
  mat3 tbnInvMat =  mat3(t,b,n);
#endif

  vec2 newTexCoord =v_tex;

  vec3 col=vec3(1.0,1.0,1.0);
  vec3 nor=n;

#if defined BUMP || defined RELIEF || defined RELIEF2
  vec3 E=normalize( v_eyeDir);
  E.y = -E.y;
#endif

#ifdef BUMP
  float height = texture2D(u_hgtMap, v_tex).r;
  height = height *u_bumpScale + u_bumpBias;
  newTexCoord = v_tex + height * E.xy;
#endif

#ifdef COLOR
  col=texture2D(u_colMap,newTexCoord).rgb;
#endif

#ifdef NORMAL
  nor=texture2D(u_norMap,newTexCoord).rgb*2.0-1.0;
  nor=normalize(tbnInvMat*nor);
#endif

  gl_FragData[1]=vec4(nor,u_reflective);
  gl_FragData[0]=vec4(col.rgb,u_shininess);
}
