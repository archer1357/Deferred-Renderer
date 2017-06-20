layout(location = 0) in vec4 a_pos;


uniform mat4 u_modelViewMat;
uniform mat4 u_projMat;
uniform vec4 u_lightPos;

// float EPSILON = 0.00;

void main() {
 vec3 pos=(u_modelViewMat*vec4(a_pos.xyz,1.0)).xyz;

 if(a_pos.w == 0.0) {
   gl_Position=u_projMat*vec4(pos,1.0);
   // gl_Position.z+=EPSILON;
 } else {
   gl_Position= u_projMat* vec4(u_lightPos.w*pos - u_lightPos.xyz, 0.0);
 }
}
