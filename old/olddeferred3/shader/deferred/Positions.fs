
layout(binding = 2) uniform sampler2D u_depthMap;

uniform mat4 u_invProjMat;
varying vec2 v_tex;

//uniform vec2 u_screenSize;
//uniform float u_zNear
//uniform float u_zFar;
//uniform mat4 u_invViewProjMat;
//uniform mat4 u_viewMat;

void main() {
  //vec2 texCoord= gl_FragCoord.xy / u_screenSize;
  
  vec2 texCoord=v_tex;
  float depth = texture2D(u_depthMap, texCoord).x;

  //works
  // vec3 ndcPos=vec3(texCoord,depth)*2.0-1.0;
  // vec4 clipPos;
  // clipPos.w=(2.0*u_zNear*u_zFar)/(u_zNear+u_zFar+(ndcPos.z*(u_zNear-u_zFar)));
  // clipPos.xyz=ndcPos*clipPos.w;
  // vec4 pos2 = u_invProjMat * clipPos;

  //fails
  // float x = texCoord.x * 2.0 - 1.0;
  // float y = (1.0 - texCoord.y) * 2.0- 1.0;
  // vec4 vProjectedPos = vec4(x, y, depth, 1.0);
  // vec3 pos =vProjectedPos.xxyz / vProjectedPos.w;

  //fails
  // //depth=depth*2.0-1.0;
  // vec4 H = vec4(texCoord.x*2.0-1.0, (1.0-texCoord.y) * 2.0 - 1.0,  depth, 1.0);
  // //vec4 D =  u_invViewProjMat *H;
  // vec4 D =  u_invProjMat *H;
  // vec4 pos = (D / D.w);

  //works
  vec4 H = vec4(vec3(texCoord,depth)*2.0-1.0, 1.0);
  vec4 D =  u_invProjMat *H;
  vec4 pos = (D / D.w);

  // gl_FragColor=vec4(100.0*abs(pos.xyz-pos2.xyz),1.0);
  gl_FragColor=pos;
}

// float color_to_float(float3 color) {
//   const float3 byte_to_float=float3(1.0,1.0/256,1.0/(256*256));
//   return dot(color,byte_to_float);
// }

// planes.x = - far_plane / (far_plane - near_plane);
// planes.y = - far_plane * near_plane / (far_plane - near_plane);
// pos.z = - planes.y / (planes.x + depth);
// pos.xy = view.xy / view.z*pos.z;
