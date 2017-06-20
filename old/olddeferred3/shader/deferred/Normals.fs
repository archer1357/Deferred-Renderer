
layout(binding = 1) uniform sampler2D u_norMap;

varying vec2 v_tex;

void main() {
  vec3 n=(normalize(texture2D(u_norMap,v_tex).xyz)+vec3(1.0))/2.0;
  gl_FragColor=vec4(n,1.0);
}