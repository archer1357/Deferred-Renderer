#version 140
#extension GL_ARB_shading_language_420pack : require

in vec2 v_tex;

layout(binding=2) uniform sampler2D u_depthMap;

void main() {
  gl_FragDepth = texture2D(u_depthMap, v_tex).r;
}