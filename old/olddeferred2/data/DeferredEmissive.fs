#ifdef GL_ES
uniform sampler2D u_colMap;
uniform sampler2D u_norMap;
uniform sampler2D u_depthMap;
#else
layout(binding = 0) uniform sampler2D u_colMap;
layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;
#endif



varying vec2 v_tex;

void main() {
  float depth = texture2D(u_depthMap, v_tex).x;
  if(depth==1.0) {discard;}

  vec4 col=texture2D(u_colMap,v_tex);
  vec4 nor=texture2D(u_norMap,v_tex);

  // if(nor!=vec3(0.0)) {
  //   discard;
  // }

  if(nor.a==0.0) {
    discard;
  }
  gl_FragColor=vec4(col.rgb*nor.a,1.0);
}
