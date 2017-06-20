#ifdef GL_ES
uniform sampler2D u_norMap;
uniform sampler2D u_depthMap;
#else
layout(binding = 1) uniform sampler2D u_norMap;
layout(binding = 2) uniform sampler2D u_depthMap;
#endif

varying vec2 v_tex;

void main() {
  if(texture2D(u_depthMap,v_tex).r == 1.0) {
    discard;
  }

  vec3 n=normalize(texture2D(u_norMap,v_tex).xyz);
  gl_FragColor=vec4(n,1.0);
  // gl_FragColor=vec4((n+vec3(1.0,1.0,1.0))/2,1.0);
}