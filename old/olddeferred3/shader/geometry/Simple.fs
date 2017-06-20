
varying vec3 v_nor;

uniform vec3 u_col;
uniform float u_shininess;
uniform float u_emissive;
uniform int u_unlit;

void main() {
  vec3 col=u_col;
  vec3 nor=normalize(v_nor);
   
  if(u_unlit==1) {
    nor=vec3(0.0,0.0,0.0);
  }  

  gl_FragData[0]=vec4(col,u_shininess);
  gl_FragData[1]=vec4(nor,u_emissive);
}
