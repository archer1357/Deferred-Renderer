
layout(binding = 0) uniform sampler2D u_colMap;
layout(binding = 2) uniform sampler2D u_depthMap;

varying vec2 v_tex;

void main() {
  if(texture2D(u_depthMap,v_tex).r == 1.0) {
    //discard;
  }

  vec3 diffuse=texture2D(u_colMap,v_tex).xyz;
  gl_FragColor=vec4(diffuse,1.0);
}
