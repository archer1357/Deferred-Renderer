layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_nor;
layout(location = 2) in vec2 a_tex;
layout(location = 3) in vec4 a_tan;

out Vertex {
  vec3 normal;
  vec2 texcoord;
  vec4 tangent;
} vertex;

void main() {
  gl_Position = vec4(a_pos,1.0);
  vertex.normal = a_nor;
  vertex.texcoord =  a_tex;
  vertex.tangent =  a_tan;
}