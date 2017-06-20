
layout(binding = 2) uniform sampler2D u_depthMap;

uniform float u_zNear
uniform float u_zFar;

varying vec2 v_tex;

void main() {
  float depth = texture2D(u_depthMap, v_tex).x;
  float linearDepth=(2.0*u_zNear)/(u_zFar+u_zNear-depth*(u_zFar-u_zNear));
  gl_FragColor=vec4(linearDepth,linearDepth,linearDepth,1.0);
}