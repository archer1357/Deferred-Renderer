attribute vec3 a_pos;
attribute vec3 a_nor;
attribute vec2 a_tex;
attribute vec4 a_tan;

uniform mat4 u_modelViewMat;
uniform mat4 u_modelViewProjMat;
uniform mat3 u_normalMat;
uniform float u_time;

varying vec2 v_tex;
varying vec3 v_nor;
varying vec3 v_tan;
varying vec3 v_bin;

varying vec3 v_posTs;
varying vec4 v_pos;

void main() {

  //
  vec3 n=normalize(u_normalMat*a_nor);
  vec3 t=normalize(u_normalMat*a_tan.xyz);
  vec3 b=normalize(a_tan.w*cross(n,t)); 

  //
  v_tex=a_tex;
  v_nor=n;
  v_tan=t;
  v_bin=b;

  //
  mat3 tbnMatrix=mat3(t.x, b.x, n.x,
                      t.y, b.y, n.y,
                      t.z, b.z, n.z);

  vec4 pos=u_modelViewMat*vec4(a_pos,1.0);
  
  v_posTs=tbnMatrix*(pos.xyz/pos.w);
  v_pos=pos;

  //
  gl_Position=u_modelViewProjMat*vec4(a_pos,1.0);
}
