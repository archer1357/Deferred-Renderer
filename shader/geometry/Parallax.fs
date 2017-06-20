#version 140
#extension GL_ARB_shading_language_420pack : require
#extension GL_ARB_separate_shader_objects : require
#extension GL_ARB_explicit_attrib_location : require

in vec4 position;
in mat3 tbnMatrix;
in vec3 positionTS;
// in float blur;

in vec2 v_tex;


in vec3 v_nor;
in vec3 v_tan;
in vec3 v_bin;

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

layout(location=0) out vec4 fragColor;
layout(location=1) out vec4 fragNormal;



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
  fragNormal = vec4(nor, 0.0);
  fragColor = vec4(col,u_shininess);

}
