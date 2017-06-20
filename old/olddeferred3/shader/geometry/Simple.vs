
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nor;

uniform mat4 u_modelViewProjMat;
uniform mat3 u_normalMat;

varying vec3 v_nor;

void main() {
  gl_Position=u_modelViewProjMat*vec4(a_pos,1.0);
  v_nor=normalize(u_normalMat*a_nor);
}
