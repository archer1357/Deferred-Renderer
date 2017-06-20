layout(location = 0) in vec3 a_pos;

uniform mat4 u_viewProjMat;

varying vec3 v_tex;

void main() {
  mat4 m=u_viewProjMat;
  
  m[0][2]=0.0;
  m[1][2]=0.0;
  m[2][2]=0.0;
  m[3][0]=0.0;
  m[3][1]=0.0;
  m[3][2]=0.0; //
  m[3][3]=0.0;

  gl_Position=(m*vec4(a_pos,1.0)).xyww;
  v_tex=a_pos;
}