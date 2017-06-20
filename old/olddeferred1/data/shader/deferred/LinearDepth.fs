
layout(binding = 2) uniform sampler2D u_depthMap;

uniform vec2 u_zNearFar;

varying vec2 v_tex;

void main() {
  float depth = texture2D(u_depthMap, v_tex).x;

  if(depth == 1.0) {
    //discard;
  }

  float znear=u_zNearFar.x;
  float zfar=u_zNearFar.y;
  float linearDepth=(2.0*znear)/(zfar+znear-depth*(zfar-znear));
  
  gl_FragColor=vec4(linearDepth,linearDepth,linearDepth,1.0);
}