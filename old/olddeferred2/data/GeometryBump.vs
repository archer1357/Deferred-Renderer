


#if defined NORMAL_VS || defined BUMP_VS || defined RELIEF_VS || defined RELIEF2_VS

#ifdef GL_ES
attribute vec3 a_pos;
attribute vec3 a_nor;
attribute vec2 a_tex;
attribute vec4 a_tan;
#else
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nor;
layout(location = 2) in vec2 a_tex;
layout(location = 3) in vec4 a_tan;
#endif

uniform mat4 u_modelViewProjMat;
uniform mat3 u_normalMat;
// uniform vec2 u_texScale;

varying vec2 v_tex;
varying vec3 v_nor;
varying vec3 v_tan;
varying vec3 v_bin;

#if defined BUMP_VS || defined RELIEF_VS || defined RELIEF2_VS
uniform mat4 u_modelViewMat;
varying vec3 v_eyeDir;
#endif

#if defined RELIEF_VS || defined RELIEF2_VS
varying vec3 position_tan;
varying vec3 eye_to_pos;
#endif


#ifdef RELIEF2_VS
varying vec4 efragCoord;
#endif

void main() {
  v_tex=a_tex;//*u_texScale;

  vec3 n=normalize(u_normalMat*a_nor);
  vec3 t=normalize(u_normalMat*a_tan.xyz);
  vec3 b=normalize(-cross(n, t)*a_tan.w);

#if defined BUMP_VS || defined RELIEF_VS || defined RELIEF2_VS

  mat3 tbnMat = mat3(t.x, b.x, n.x,
                     t.y, b.y, n.y,
                     t.z, b.z, n.z);

  vec4 pos=u_modelViewMat*vec4(a_pos,1.0);
  //v_eyeDir=-normalize(pos.xyz / pos.w);
  //v_eyeDir=tbnMat*v_eyeDir;
  v_eyeDir=tbnMat*-pos.xyz;

#endif

#if defined RELIEF_VS || defined RELIEF2_VS
   position_tan = vec3(a_pos) * tbnMat;
   eye_to_pos = pos.xyz;
#endif

#ifdef RELIEF2_VS
efragCoord = pos;
#endif

  v_nor=n;
  v_tan=t;
  v_bin=b;

  gl_Position=u_modelViewProjMat*vec4(a_pos,1.0);
}

#endif
