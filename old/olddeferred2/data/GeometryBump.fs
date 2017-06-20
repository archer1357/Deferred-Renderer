#ifdef GL_ES
#extension GL_EXT_draw_buffers : require
#endif

#if defined NORMAL_FS || defined BUMP_FS || defined RELIEF2_FS
#ifdef GL_ES
uniform sampler2D u_norMap;
#else
layout(binding = 1) uniform sampler2D u_norMap;
#endif
#endif

#if defined NORMAL_FS || defined BUMP_FS || defined RELIEF_FS || defined RELIEF2_FS
#ifdef GL_ES
uniform sampler2D u_colMap;
#else
layout(binding = 0) uniform sampler2D u_colMap;
#endif


varying vec2 v_tex;
varying vec3 v_nor;
varying vec3 v_tan;
varying vec3 v_bin;


uniform float u_shininess;

#if defined BUMP_FS || defined RELIEF2_FS
#ifdef GL_ES
uniform sampler2D u_hgtMap;
#else
layout(binding = 2) uniform sampler2D u_hgtMap;
#endif
#endif

#if defined BUMP_FS
uniform float u_bumpScale;
uniform float u_bumpBias;
#endif

#if defined BUMP_FS || defined RELIEF_FS
varying vec3 v_eyeDir;
#endif

#if defined RELIEF_FS || defined RELIEF2_FS
varying vec3 eye_to_pos;
#endif


#ifdef RELIEF2_FS
varying vec4 efragCoord;

varying vec3 v_eyeDir;
#endif


#ifdef RELIEF2_FS


varying vec3 position_tan;
#define SHADOWS         1
#define DEPTH_CORRECT   1
#define SHOW_DEPTH      0

#define LINEAR_STEPS    60
#define BINARY_STEPS    16
uniform vec2 u_zNearFar;
uniform float u_scale2;

float linearSearch(vec2 A, vec2 B) {
  float t = 0.0;

  for(int i = 0; i < LINEAR_STEPS; i++) {
    t += 1.0 / LINEAR_STEPS;

    float d = 1.0-texture2D(u_hgtMap, mix(A, B, t)).r;

    if(t > d) break;
  }

  return t;
}

float binarySearch(vec2 A, vec2 B, float a, float b) {
  float depth;

  for(int i = 0; i < BINARY_STEPS; i++) {
    depth = mix(a, b, 0.5);

    float d =  1.0-texture2D(u_hgtMap, mix(A, B, depth)).r;


    if(d > depth)
      a = depth;
    else
      b = depth;
  }

    return depth;
  }

float fullSearch(vec2 A, vec2 B) {
    float depth = linearSearch(A, B);
    return binarySearch(A, B, depth-(1.0 / LINEAR_STEPS), depth);
}

#endif

#ifdef RELIEF_FS
layout(binding = 1) uniform sampler2D u_reliefMap;

varying vec3 position_tan;
#define SHADOWS         1
//#define DEPTH_CORRECT   1
#define SHOW_DEPTH      0

#define LINEAR_STEPS    60
#define BINARY_STEPS    16
uniform vec2 u_zNearFar;
uniform float u_scale;

float linearSearch(vec2 A, vec2 B) {
  float t = 0.0;

  for(int i = 0; i < LINEAR_STEPS; i++) {
    t += 1.0 / LINEAR_STEPS;

    float d = texture2D(u_reliefMap, mix(A, B, t)).a;

    if(t > d) break;
  }

  return t;
}

float binarySearch(vec2 A, vec2 B, float a, float b) {
  float depth;

  for(int i = 0; i < BINARY_STEPS; i++) {
    depth = mix(a, b, 0.5);

    float d =  texture2D(u_reliefMap, mix(A, B, depth)).a;


    if(d > depth)
      a = depth;
    else
      b = depth;
  }

    return depth;
  }

float fullSearch(vec2 A, vec2 B) {
    float depth = linearSearch(A, B);
    return binarySearch(A, B, depth-(1.0 / LINEAR_STEPS), depth);
}



#endif


void main() {
  vec3 t=normalize(v_tan);
  vec3 b=normalize(v_bin);
  vec3 n=normalize(v_nor);
  mat3 tbnInvMat =  mat3(t,b,n);

  vec2 newTexCoord =v_tex;

  vec3 col=vec3(1.0);
  vec3 nor=n;

#if defined BUMP_FS || defined RELIEF_FS || defined RELIEF2_FS
  vec3 E=normalize(v_eyeDir);
#endif



#ifdef RELIEF2_FS
  vec2 A = v_tex;
  float scale=u_scale2;
  vec3 to_eye=E;
  vec3 V = (to_eye / -to_eye.z) * scale;
  vec2 B = A + V.xy;

  float depth = fullSearch(A, B);


  vec3 P = vec3(mix(A, B, depth), depth);

  vec3 P_tan = position_tan + (to_eye / -to_eye.z) * scale * depth;
  vec4 diffuse_col = texture2D(u_colMap, P.xy);
  col = diffuse_col.rgb;

  nor = texture2D(u_norMap, P.xy).rgb;

  nor = normalize((nor - 0.5) * 2.0);
  // nor.y=-nor.y;
  nor=normalize(tbnInvMat*nor);


#if DEPTH_CORRECT

 float near = u_zNearFar.x;
 float far  = u_zNearFar.y;
 float p_eye_z = eye_to_pos.z + normalize(eye_to_pos).z * scale * depth;
 gl_FragDepth = ((-far / (far - near)) * p_eye_z + (-far * near / (far - near))) / -p_eye_z;
#endif
#endif



#ifdef RELIEF_FS
  vec2 A = v_tex;
  float scale=u_scale;
  vec3 to_eye=E;
  vec3 V = (to_eye / -to_eye.z) * scale;
  vec2 B = A + V.xy;

  float depth = fullSearch(A, B);


  vec3 P = vec3(mix(A, B, depth), depth);

  vec3 P_tan = position_tan + (to_eye / -to_eye.z) * scale * depth;
  vec4 diffuse_col = texture2D(u_colMap, P.xy);
  col = diffuse_col.rgb;

  nor = texture2D(u_reliefMap, P.xy).rgb;

  nor = normalize((nor - 0.5) * 2.0);
  nor.y=-nor.y;
  nor=normalize(tbnInvMat*nor);

//   to_light = (light_pos - vec3(gl_Vertex)) * TBN;
//   vec3 p_to_light = (position_tan + to_light) - P_tan;
//   float n_dot_l = max(dot(norm, normalize(p_to_light)), 0.0);
//   if(n_dot_l > 0.0) {
// #if SHADOWS
//     vec3 l_entry = P + (p_to_light / p_to_light.z) * scale * depth;
//     vec3 l_exit  = l_entry + (p_to_light / -p_to_light.z) * scale;
//     float l_depth = fullSearch(l_entry.xy, l_exit.xy);

//     if(l_depth < depth-0.05)  {//in shadow
//       col += diffuse_col * 0.2 * n_dot_l;
//     } else {
//       col += diffuse_col * n_dot_l;
//       vec3 H = normalize(p_to_light + to_eye);
//       col += vec4(0.5, 0.5, 0.5, 1.0) * pow(max(dot(norm,H),0.0), 64.0);
//     }
// #else
//     col += diffuse_col * n_dot_l;

//     vec3 H = normalize(p_to_light + to_eye);
//     col += vec4(0.5, 0.5, 0.5, 1.0) * pow(max(dot(norm,H),0.0), 64.0);
// #endif
  // }

#if DEPTH_CORRECT

 float near = u_zNearFar.x;
 float far  = u_zNearFar.y;
 float p_eye_z = eye_to_pos.z + normalize(eye_to_pos).z * scale * depth;
 gl_FragDepth = ((-far / (far - near)) * p_eye_z + (-far * near / (far - near))) / -p_eye_z;
#endif
#endif

#ifdef BUMP_FS
  float height = texture2D(u_hgtMap, v_tex).r;
  height = height *u_bumpScale + u_bumpBias;
  newTexCoord = v_tex + height * E.xy;
#endif

#if defined NORMAL_FS || defined BUMP_FS
  col=texture2D(u_colMap,newTexCoord).rgb;
  nor=texture2D(u_norMap,newTexCoord).rgb*2.0-1.0;
  nor=normalize(tbnInvMat*nor);
#endif
//col.rgb=vec3(1.0,1.0,1.0);
//nor=v_nor;
  gl_FragData[1]=vec4(nor,0.0);
  gl_FragData[0]=vec4(col.rgb,u_shininess);
  //gl_FragData[1]=vec4(col.rgb+mycol.rgb,1.0);
}

#endif
