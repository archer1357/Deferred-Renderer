
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nor;
layout(location = 2) in vec2 a_tex;


layout(location = 3) in vec4 a_tan;

uniform mat4 u_modelViewMat;
uniform mat4 u_modelViewProjMat;
uniform mat3 u_normalMat;

varying vec2 v_tex;
varying vec3 v_nor;
varying vec3 v_tan;
varying vec3 v_bin;
varying vec3 v_eyeDir;
varying vec3 position_tan;
varying vec3 eye_to_pos;
varying vec4 efragCoord;

void main() {
  v_tex=a_tex;

  vec3 n=normalize(u_normalMat*a_nor);
  vec3 t=normalize(u_normalMat*a_tan);
  vec3 b=normalize(-cross(n, t)    ); //*a_tan.w

  mat3 tbnMat = mat3(t.x, b.x, n.x,
                     t.y, b.y, n.y,
                     t.z, b.z, n.z);

  vec4 pos=u_modelViewMat*vec4(a_pos,1.0);
  v_eyeDir=tbnMat*(pos.xyz / pos.w);

  v_nor=n;
  v_tan=t;
  v_bin=b;

  position_tan = vec3(a_pos) * tbnMat;
  eye_to_pos = pos.xyz;
  efragCoord = pos;

  gl_Position=u_modelViewProjMat*vec4(a_pos,1.0);
}
