#ifdef GL_ES
uniform sampler2D u_depthMap;
#else
layout(binding = 2) uniform sampler2D u_depthMap;
#endif


varying vec2 v_tex;

void main() {
  float depth=texture2D(u_depthMap, v_tex).x;
  if(depth == 1.0) {
    discard;
  }
  gl_FragColor = vec4(1.0,1.0,1.0,1.0);
}
