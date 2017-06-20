#ifdef GL_ES
attribute vec3 a_pos;
attribute vec3 a_nor;
#else
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nor;
#endif

uniform mat4 u_modelViewProjMat;
uniform mat3 u_normalMat;

varying vec3 v_nor;

void main() {
  gl_Position=u_modelViewProjMat*vec4(a_pos,1.0);
  v_nor=u_normalMat*a_nor;
}
