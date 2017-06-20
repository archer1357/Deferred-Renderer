
#ifdef GL_ES
#extension GL_EXT_draw_buffers : require
#endif

varying vec3 v_nor;
uniform vec3 u_col;
uniform float u_shininess;
uniform float u_reflective;
uniform float u_emissive;
uniform bool u_unlit;

// float packColor(vec3 color) {
//     return color.r + color.g * 256.0 + color.b * 256.0 * 256.0;
// }


void main() {
  vec4 col=vec4(u_col,u_shininess);
  vec4 nor=vec4(v_nor,u_emissive);
  // nor.a=u_shininess;
  // col.a+=u_shininess/2.0;
  // col.a=-0.5;
  // if(u_emissive) {
  //   col.a+=0.5;
  // }
  // col.a=col.a & 1.0;
  
  if(u_unlit) {
    nor.rgb=vec3(0.0,0.0,0.0);
  }

  gl_FragData[0]=col;
  // gl_FragData[0]=vec4(1.0,1.0,1.0,0.0);
  gl_FragData[1]=nor;
  // gl_FragData[1]=vec4(v_nor,packColor(u_col));
}
