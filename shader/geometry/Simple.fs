#version 130
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require
 
in vec3 v_nor;

#ifdef VERTEX_COLOR
in vec3 v_col;
#endif

uniform vec3 u_col;
uniform float u_shininess;
uniform float u_emissive;
uniform int u_unlit;

layout(location=0) out vec4 fragColor;
layout(location=1) out vec4 fragNormal;


void main() {
  vec3 col=u_col;
  
#ifdef VERTEX_COLOR
  col*=v_col;
#endif

  vec3 nor=normalize(v_nor);
   
  if(u_unlit==1) {
    nor=vec3(0.0,0.0,0.0);
  }  
// 

  fragColor=vec4(col,u_shininess);
  fragNormal=vec4(nor,u_emissive);
}
