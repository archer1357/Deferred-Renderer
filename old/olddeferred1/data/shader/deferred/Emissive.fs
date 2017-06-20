
layout(binding = 0) uniform sampler2D u_colMap;
layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;

varying vec2 v_tex;

void main() {
  float depth = texture2D(u_depthMap, v_tex).x;
  if(depth==1.0) {discard;}

  vec4 nor=texture2D(u_norMap,v_tex);

  if(nor.xyz!=vec3(0.0,0.0,0.0)) {
    discard;
  }

  vec4 col=texture2D(u_colMap,v_tex);
  gl_FragColor=col;
}
