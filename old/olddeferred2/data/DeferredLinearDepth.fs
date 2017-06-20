#ifdef GL_ES
uniform sampler2D u_depthMap;
#else
layout(binding = 2) uniform sampler2D u_depthMap;
#endif

uniform vec2 u_zNearFar;

varying vec2 v_tex;

void main() {
  float z = texture2D(u_depthMap, v_tex).x;

  if(z == 1.0) {
    discard;
  }

  float znear=u_zNearFar.x;
  float zfar=u_zNearFar.y;
  float linearDepth=(2.0*znear)/(zfar+znear-z*(zfar-znear));
  gl_FragColor=vec4(linearDepth,linearDepth,linearDepth,1.0);
}