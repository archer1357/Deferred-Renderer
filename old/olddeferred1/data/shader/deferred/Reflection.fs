#extension GL_NV_shadow_samplers_cube : enable

layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;
layout(binding = 3) uniform samplerCube u_envMap;

uniform mat4 u_invProjMat;
uniform mat4 u_invViewMat;

in vec2 v_tex;

void main() {
  float depth = texture2D(u_depthMap, v_tex).x;
  if(depth==1.0) {discard;}
  
  vec4 nor2=texture2D(u_norMap,v_tex);  
  vec3 nor=nor2.xyz;
  float reflective=abs(nor2.a);

  if(nor==vec3(0.0) || reflective==0.0) {
    discard;
  }
  
  
  //
  nor=normalize(nor);
  
  vec4 H = vec4(vec3(v_tex,depth)*2.0-1.0, 1.0);
  vec4 D = u_invProjMat*H;
  vec3 pos = (D / D.w).xyz;
  
  vec3 eyeDir=normalize(pos);

  vec3 R = (u_invViewMat*vec4(reflect(eyeDir, nor),0.0)).xyz;
  vec3 col=textureCube(u_envMap, R).rgb;
  
  gl_FragData[0] = vec4(col,reflective);
}
