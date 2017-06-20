#ifdef GL_ES
uniform sampler2D u_depthMap;
#else
layout(binding = 2) uniform sampler2D u_depthMap;
#endif


varying vec2 v_tex;

void main() {
  gl_FragDepth = texture2D(u_depthMap, v_tex).x;
}
