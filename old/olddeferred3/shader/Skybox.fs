layout(binding = 3) uniform samplerCube u_cubeMap;

varying vec3 v_tex;

void main() {
  vec4 c=textureCube(u_cubeMap, v_tex);
  gl_FragColor =c;
}
