
varying vec3 v_nor;
varying vec3 v_eye;
 
uniform vec3 u_col;
uniform float u_shininess;
uniform float u_emissive;
uniform int u_unlit;
uniform mat4 u_invViewMat;

layout(binding = 3) uniform samplerCube u_envMap;

void main() {
  vec3 nor=normalize(v_nor);
  vec3 eye=normalize(v_eye);

  vec3 R = reflect(eye, nor);
  R=(u_invViewMat*vec4(R,0.0)).xyz;
  vec3 env=textureCube(u_envMap, R).rgb;
  env=u_col*env*env; 
  vec3 col=mix(u_col,env,0.7);

  if(u_unlit==1) {
    nor.rgb=vec3(0.0,0.0,0.0);
  }

  gl_FragData[0]=vec4(col,u_shininess);
  gl_FragData[1]=vec4(nor,u_emissive);  
}


