#version 140
#extension GL_ARB_shading_language_420pack : require
#extension GL_NV_shadow_samplers_cube : enable
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(binding = 3) uniform samplerCube u_cubeMap;

in vec3 v_tex;

layout(location=0) out vec4 fragColor;

void main() {
  vec4 c=textureCube(u_cubeMap, v_tex);
  fragColor =c;
}
