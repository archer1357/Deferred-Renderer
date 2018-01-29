#version 440

layout(location = 0) in vec3 a_pos;

uniform mat4 u_modelViewMat;

out int vertexID;

void main() {
    vertexID=gl_VertexID;
    gl_Position=u_modelViewMat*a_pos;
}
