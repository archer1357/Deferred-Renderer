precision highp float;

uniform mat3 _dLight; //[0]=dir

uniform sampler2D u_colMap;
uniform sampler2D u_reliefMap;

varying vec2 v_tex;

varying vec3 v_nor;
varying vec3 v_tan;
varying vec3 v_bin;

varying vec3 v_posTs;
varying vec4 v_pos;

void main() {
  float u_bumpScale=0.04;
  float u_bumpBias=-0.03;

  vec4 norHgt;
  vec2 viewTS = normalize(-v_posTs).xy;
  vec2 TexCoord = v_tex;
  
  //from Angelo "Encelo" Theodorou's iterative parallax shader
  for(int i=0;i<4;i++) {
    norHgt=texture2D(u_reliefMap, TexCoord);
    //norHgt.a=1.0-norHgt.a;

    float height = norHgt.a * u_bumpScale + u_bumpBias;
    TexCoord += height * norHgt.z * viewTS;
  }

  vec3 t=normalize(v_tan);
  vec3 b=normalize(v_bin);
  vec3 n=normalize(v_nor);

  mat3 invTbnMat= mat3(t,b,n);
  vec3 nor = normalize(2.0 * norHgt.rgb - 1.0);
  nor=normalize(invTbnMat*nor);

  vec3 col=texture2D(u_colMap, TexCoord).rgb;
  
  //
  vec3 lightDir=normalize(-_dLight[0]);
  vec3 reflectVec=reflect(-lightDir,nor);
  float NdotL = max(0.0,dot(nor,lightDir));
  gl_FragColor=vec4(col*NdotL,1.0);
}
