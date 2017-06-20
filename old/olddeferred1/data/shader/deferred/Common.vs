
layout(location = 0) in vec3 a_pos;

varying vec2 v_tex;

void main() {
  gl_Position = vec4(a_pos,1.0);
  v_tex = a_pos.xy/2.0 + 0.5;
}
