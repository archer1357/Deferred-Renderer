#version 140

#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(binding = 1) uniform sampler2D u_norMap;

in vec2 v_tex;
layout(location = 0) out vec4 fragColor;


void main() {
  vec3 n=(normalize(texture2D(u_norMap,v_tex).xyz)+vec3(1.0))/2.0;
  fragColor=vec4(n,1.0);
}