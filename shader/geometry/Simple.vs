#version 130
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nor;

#ifdef VERTEX_COLOR
layout(location = 4) in vec3 a_col;
#endif

uniform mat4 u_modelViewProjMat;
uniform mat3 u_normalMat;

out vec3 v_nor;

#ifdef VERTEX_COLOR
out vec3 v_col;
#endif

void main() {
  gl_Position=u_modelViewProjMat*vec4(a_pos,1.0);
  v_nor=normalize(u_normalMat*a_nor);
  
#ifdef VERTEX_COLOR
  v_col=a_col;
#endif

}
