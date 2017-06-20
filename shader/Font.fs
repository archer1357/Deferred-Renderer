#version 140
#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(binding=4) uniform sampler2D u_colMap;

in vec2 v_tex;

layout(location=0) out vec4 fragColor;


void main() {
  float d=texture2D(u_colMap,v_tex).r;
  fragColor=vec4(vec3(1.0),d);
}
