#version 140

#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(binding = 2) uniform sampler2D u_depthMap;

uniform float u_zNear
uniform float u_zFar;

in vec2 v_tex;
layout(location = 0) out vec4 fragColor;


void main() {
  float depth = texture2D(u_depthMap, v_tex).x;
  float linearDepth=(2.0*u_zNear)/(u_zFar+u_zNear-depth*(u_zFar-u_zNear));
  fragColor=vec4(linearDepth,linearDepth,linearDepth,1.0);
}