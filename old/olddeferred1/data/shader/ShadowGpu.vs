layout(location = 0) in vec4 a_pos;

uniform mat4 u_modelViewMat;

void main() {
  gl_Position=u_modelViewMat*a_pos;
}