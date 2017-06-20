
layout(location = 0) in vec2 a_pos;
layout(location = 2) in vec2 a_tex;
uniform mat4 u_projMat;

varying vec2 v_tex;
//
void main() {
  gl_Position = u_projMat*vec4(a_pos.xy,0.0,1.0);
  v_tex=a_tex;
}

