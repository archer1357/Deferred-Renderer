#extension GL_NV_shadow_samplers_cube : enable

#ifdef GL_ES
uniform samplerCube u_cubeMap;
#else
layout(binding = 3) uniform samplerCube u_cubeMap;
#endif

in vec3 v_tex;

void main() {
  vec4 c=textureCube(u_cubeMap, v_tex);

#ifdef BLUISH
  c=c.bgra;
  c.b*=0.9;
  c.g*=0.7;
  c.r+=c.b*0.3;
  c.b+=0.065;
#endif
  gl_FragData[0] =c;
}
