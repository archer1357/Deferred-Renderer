
#ifdef GL_ES
#extension GL_EXT_draw_buffers : require
#endif

varying vec3 v_nor;
uniform vec3 u_col;
uniform float u_shininess;
uniform float u_reflective;
uniform bool u_emissive;

// float packColor(vec3 color) {
//     return color.r + color.g * 256.0 + color.b * 256.0 * 256.0;
// }
vec4 packFloatToVec4i(const float value)
{
	vec4 bitSh = vec4(256.0 * 256.0 * 256.0, 256.0 * 256.0, 256.0, 1.0);
	vec4 bitMsk = vec4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);
	vec4 res = fract(value * bitSh);
	res -= res.xxyz * bitMsk;
	return res;
}

void main() {
  vec4 col=vec4(u_col,u_shininess);
  vec4 nor=vec4(v_nor,u_reflective);
   
  if(u_emissive) {
    nor.rgb=vec3(0.0,0.0,0.0);
    
  }
  
  gl_FragData[0]=col;
  gl_FragData[1]=nor;
}
