#version 140

#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;
layout(binding = 3) uniform samplerCube u_envMap;

uniform mat4 u_invProjMat;
uniform mat4 u_invViewMat;

in vec2 v_tex;
layout(location = 0) out vec4 fragColor;


void main() {
  float depth = texture2D(u_depthMap, v_tex).x;
  
  vec4 nor2=texture2D(u_norMap,v_tex);  
  vec3 nor=normalize(nor2.xyz);

  vec4 H = vec4(vec3(v_tex,depth)*2.0-1.0, 1.0);
  vec4 D = u_invProjMat*H;
  vec3 pos = (D / D.w).xyz;
  
  vec3 eyeDir=normalize(pos);

  vec3 R = (u_invViewMat*vec4(reflect(eyeDir, nor),0.0)).xyz;
  vec3 col=textureCube(u_envMap, R).rgb;
  
  fragColor = vec4(col,1.0);
}
