#version 140

#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(binding = 0) uniform sampler2D u_colMap;
layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;

in vec2 v_tex;

layout(location = 0) out vec4 fragColor;

void main() {
  float depth = texture2D(u_depthMap, v_tex).x;
  
  if(depth==1.0) {
    discard;
  }

  vec4 nor2=texture2D(u_norMap,v_tex);
  vec3 nor=nor2.xyz;
  float emissive=nor2.a;
  
  if(emissive == 0.0) {
    discard;
  }

  vec4 col=texture2D(u_colMap,v_tex)*emissive;
  fragColor=col;
}
