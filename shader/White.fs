#version 130
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(location=0) out vec4 fragColor;

void main() {
  fragColor=vec4(1.0,1.0,1.0,1.0);
}