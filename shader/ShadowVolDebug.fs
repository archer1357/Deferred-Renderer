#version 130
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

in vec4 vertex_color;
layout(location=0) out vec4 fragColor;


void main() {
  fragColor=vertex_color;
}