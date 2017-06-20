
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nor;
layout(location = 2) in vec2 a_tex;


#if defined BUMP || defined RELIEF || defined RELIEF2
layout(location = 3) in vec4 a_tan;
// layout(location = 3) in vec3 a_tan;
#endif

#if defined BUMP || defined RELIEF || defined RELIEF2
uniform mat3 u_normalMat;
uniform mat4 u_modelViewMat;
#endif

uniform mat4 u_modelViewProjMat;

varying vec2 v_tex;
varying vec3 v_nor;

#if defined BUMP || defined RELIEF || defined RELIEF2
varying vec3 v_tan;
varying vec3 v_bin;
varying vec3 v_eyeDir;
#endif

void main() {
  v_tex=a_tex;

  vec3 n=normalize(u_normalMat*a_nor);
  vec3 t=normalize(u_normalMat*a_tan.xyz);





  vec3 b=normalize(-a_tan.w*cross(n,t));


#if defined BUMP || defined RELIEF || defined RELIEF2
  // mat3 tbnMat = mat3(t.x, b.x, n.x,
  //                    t.y, b.y, n.y,
  //                    t.z, b.z, n.z);
  mat3 tbnMat= transpose(mat3(t,b,n));
  vec4 pos=u_modelViewMat*vec4(a_pos,1.0);
  // v_eyeDir=normalize(tbnMat*(pos.xyz / pos.w));
  v_eyeDir=normalize(tbnMat*pos.xyz);
  // v_eyeDir=normalize(pos.xyz / pos.w);
  // v_eyeDir=normalize(-pos.xyz);

#endif

  v_nor=n;
  v_tan=t;
  v_bin=b;

  gl_Position=u_modelViewProjMat*vec4(a_pos,1.0);
}
