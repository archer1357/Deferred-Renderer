layout(location = 0) in vec4 a_pos;

uniform mat4 u_projMat;
uniform mat4 u_modelViewMat;

#ifdef POINT
uniform vec4 u_lightPos;
#endif

#ifdef DIRECTIONAL
uniform vec4 u_lightDir;
#endif

void main() {
  vec3 pos=(u_modelViewMat*vec4(a_pos.xyz,1.0)).xyz;

  if(a_pos.w == 0.0) {
    gl_Position=u_projMat*vec4(pos,1.0);
  } else {
#ifdef POINT
    gl_Position= u_projMat* vec4(u_lightPos.w*pos - u_lightPos.xyz, 0.0);
#endif
 
#ifdef DIRECTIONAL
    gl_Position= u_projMat* vec4(u_lightDir.xyz, 0.0);
#endif
  }
}
