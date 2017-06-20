#version 130
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 0) in vec4 a_pos_tex;
uniform mat4 u_proj2dMat;
out vec2 v_tex;

void main() {
  gl_Position = u_proj2dMat*vec4(a_pos_tex.xy,0.0,1.0);
  v_tex=a_pos_tex.zw;
}
