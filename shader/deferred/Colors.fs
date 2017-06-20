#version 140

#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(binding = 0) uniform sampler2D u_colMap;
layout(binding = 2) uniform sampler2D u_depthMap;

in vec2 v_tex;
layout(location = 0) out vec4 fragColor;


void main() {
  if(texture2D(u_depthMap,v_tex).r == 1.0) {
    //discard;
  }

  vec3 diffuse=texture2D(u_colMap,v_tex).xyz;
  fragColor=vec4(diffuse,1.0);
}
