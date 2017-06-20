#ifdef GL_ES
attribute vec3 a_pos;
#else
layout(location = 0) in vec3 a_pos;
#endif

uniform mat4 u_viewRotProjMat;
varying vec3 v_tex;

void main() {
  gl_Position=u_viewRotProjMat*vec4(a_pos*5.0,1.0);
  v_tex=a_pos;
}