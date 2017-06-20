
layout(binding = 4) uniform sampler2D u_colMap;

varying vec2 v_tex;

void main() {
//v_tex.y*=-1.0;
  float diffuse=texture2D(u_colMap,v_tex).r;
  vec3 c=diffuse*vec3(1.0,0.99,0.95);
 // vec3 c=vec3(1.0,1.0,0.0);
  gl_FragColor=vec4(c,diffuse);
}
