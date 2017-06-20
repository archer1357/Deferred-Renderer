
layout(binding = 4) uniform sampler2D u_colMap;

varying vec2 v_tex;

void main() {
v_tex.y*=-1.0;
float r=texture2D(u_colMap,v_tex);
  gl_FragColor=vec4(r,r,r,r);
}
