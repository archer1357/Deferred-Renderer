layout(location = 0) in vec3 a_pos;

uniform mat4 u_modelViewMat;

void main() {
  gl_Position=u_modelViewMat*vec4(a_pos,1.0);
}