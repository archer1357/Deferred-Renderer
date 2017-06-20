
layout(binding = 2) uniform sampler2D u_depthMap;

varying vec2 v_tex;

void main() {
  gl_FragDepth = texture2D(u_depthMap, v_tex).x;
}
