layout(location = 0) in vec4 a_pos;

uniform mat4 u_modelViewMat;

#ifdef SMOOTH
layout(location = 1) in vec3 a_nor;

uniform mat3 u_normalMat;

out Vertex {
  vec3 normal;
} vertex;

#endif

void main() {
  gl_Position=u_modelViewMat*a_pos;
  
#ifdef SMOOTH
  vertex.normal=normalize(u_normalMat*a_nor);
#endif
}