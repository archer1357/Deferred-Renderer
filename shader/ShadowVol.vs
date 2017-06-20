#version 130
#extension GL_ARB_explicit_attrib_location : enable

layout(location = 0) in vec4 a_pos;

//out Vertex {
//  vec3 normal;
//} vertex;

uniform mat4 u_modelViewMat;


void main() {
  gl_Position=u_modelViewMat*a_pos;
}