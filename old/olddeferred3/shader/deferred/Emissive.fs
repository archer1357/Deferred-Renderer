
layout(binding = 0) uniform sampler2D u_colMap;
layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;

varying vec2 v_tex;

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
  gl_FragColor=col;
}
