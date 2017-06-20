varying vec4 position;
varying mat3 tbnMatrix;
varying vec3 positionTS;
// varying float blur;

varying vec2 v_tex;


varying vec3 v_nor;
varying vec3 v_tan;
varying vec3 v_bin;

layout(binding = 0) uniform sampler2D u_colMap;
#ifdef NORMAL_HEIGHT_MAPS
layout(binding = 1) uniform sampler2D u_normalMap;
layout(binding = 2) uniform sampler2D u_heightMap;
#else
layout(binding = 3) uniform sampler2D u_reliefMap;
#endif

uniform float u_bumpScale;
uniform float u_bumpBias;

uniform float u_shininess;


void main() {
  vec4 norHgt;
  vec2 TexCoord = v_tex;
  vec2 viewTS = normalize(-positionTS).xy;

  // Flip the view vector vertically to match the GL coordinate system
  viewTS.y = -viewTS.y;

  for(int i = 0; i < 4; i++) {
#ifdef NORMAL_HEIGHT_MAPS
    norHgt.rgb=texture2D(u_normalMap, TexCoord).rgb;
    norHgt.a=texture2D(u_heightMap, TexCoord).r;
#else
    norHgt = texture2D(u_reliefMap, TexCoord);
#endif
    float a=norHgt.a;

   
      //a=1.0-a;
    

    float height = a * u_bumpScale + u_bumpBias;
    TexCoord += height * norHgt.z * viewTS;
  }

#ifdef NORMAL_HEIGHT_MAPS
    norHgt.rgb=texture2D(u_normalMap, TexCoord).rgb;
    norHgt.a=texture2D(u_heightMap, TexCoord).r;
#else
    norHgt = texture2D(u_reliefMap, TexCoord);
#endif

  vec3 nor = normalize(2.0 * norHgt.rgb - 1.0); // decoding normal map
// vec3 normal2 = normalize(tbnMatrix*normal); // Matrix after vector (like using transpose)
  // normal2=normal2 * 0.5 + 0.5;
  vec3 t=normalize(v_tan);
  vec3 b=normalize(v_bin);
  vec3 n=normalize(v_nor);
  mat3 tbnInvMat =  mat3(t,b,n);
  nor=normalize(tbnInvMat*nor);

  // gl_FragData[1] = vec4(normalize(normal) * 0.5 + 0.5, blur);
  vec3 col=texture2D(u_colMap, TexCoord).rgb;
  gl_FragData[1] = vec4(nor, 0.0);
  gl_FragData[0] = vec4(col,u_shininess);

}
