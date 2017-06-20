//todo fix weird bug when disabling front cap, maybe preprocessor error or shader manager insert defines error
// rearranging front cap and back cap functions reverses the problem
// partially solved: doesn't like having the emitvertex & endprimitive inside functions other than main

#version 330
//#extension GL_ARB_geometry_shader4 : enable

layout(triangles_adjacency) in;

#if defined POINT
# ifdef ZPASS
#  define OUT_MAX_VERTS 12
# else
#  define OUT_MAX_VERTS 18
# endif
#elif defined DIRECTIONAL
# ifdef ZPASS
#  define OUT_MAX_VERTS 9
# else
#  define OUT_MAX_VERTS 12
# endif
#endif

#ifdef DEBUG
//layout(line_strip, max_vertices=OUT_MAX_VERTS) out;
layout(triangle_strip, max_vertices=OUT_MAX_VERTS) out;
#else
layout(triangle_strip, max_vertices=OUT_MAX_VERTS) out;
#endif

#ifdef DEBUG
out vec4 vertex_color;
#endif

uniform mat4 u_projMat; 

#ifdef POINT
uniform vec4 u_lightPos;
#endif

#ifdef DIRECTIONAL
uniform vec4 u_lightDir;
#endif


vec3 calcTriNor(int i0, int i1, int i2) {
  vec3 e0=gl_in[i1].gl_Position.xyz - gl_in[i0].gl_Position.xyz;
  vec3 e1=gl_in[i2].gl_Position.xyz - gl_in[i0].gl_Position.xyz;
  vec3 n=cross(e0,e1);
  //n=normalize(n);
  return n;
}

bool isFaceLit(int i0, int i1, int i2, vec3 n) {
#ifdef POINT
  vec3 d0 = u_lightPos.xyz-u_lightPos.w*gl_in[i0].gl_Position.xyz;
  vec3 d1 = u_lightPos.xyz-u_lightPos.w*gl_in[i1].gl_Position.xyz;
  vec3 d2 = u_lightPos.xyz-u_lightPos.w*gl_in[i2].gl_Position.xyz;

# ifdef BACKFACE
    d0*=-1.0;
    d1*=-1.0;
    d2*=-1.0;
# endif

  return (dot(n,d0)>0.0 || dot(n,d1)>0.0 || dot(n,d2)>0.0);
#endif

#ifdef DIRECTIONAL
  vec3 d=u_lightDir.xyz;

# ifndef BACKFACE
    d*=-1.0;
# endif

  return (dot(n,d)>0.0);
#endif
}


#ifndef ZPASS


# ifdef POINT
void emitBackCap(
#  ifdef BACKFACE
  int i2,int i1,int i0
#  else
  int i0,int i1,int i2
#  endif
#  ifdef DEBUG
  ,vec3 col
#  endif
  ) {  
  vec4 v0=vec4(u_lightPos.w*gl_in[i0].gl_Position.xyz-u_lightPos.xyz,0.0);
  vec4 v1=vec4(u_lightPos.w*gl_in[i1].gl_Position.xyz-u_lightPos.xyz,0.0);
  vec4 v2=vec4(u_lightPos.w*gl_in[i2].gl_Position.xyz-u_lightPos.xyz,0.0);
 
  //2 
  gl_Position = u_projMat*v2;
#  ifdef DEBUG
  vertex_color = vec4(col,1.0);
#  endif
  EmitVertex();

  //1 
  gl_Position = u_projMat*v1;
#  ifdef DEBUG
  vertex_color = vec4(col,1.0);
#  endif
  EmitVertex();

  //0 
  gl_Position = u_projMat*v0;
#  ifdef DEBUG
  vertex_color = vec4(col,1.0);
#  endif
  EmitVertex();

  //
  EndPrimitive();
}
# endif

void emitFrontCap(
# ifdef BACKFACE
  int i2,int i1,int i0
# else
  int i0,int i1,int i2
# endif
# ifdef DEBUG
  ,vec3 col
# endif
  ) {
  vec4 v0=gl_in[i0].gl_Position;
  vec4 v1=gl_in[i1].gl_Position;
  vec4 v2=gl_in[i2].gl_Position;
  
  //0
  gl_Position = u_projMat*v0;
# ifdef DEBUG
  vertex_color = vec4(col,1.0);
# endif
  EmitVertex();

  //1
  gl_Position = u_projMat*v1;
# ifdef DEBUG
  vertex_color = vec4(col,1.0);
# endif
  EmitVertex();

  //2
  gl_Position = u_projMat*v2;
# ifdef DEBUG
  vertex_color = vec4(col,1.0);
# endif
  EmitVertex();
  
  //
  EndPrimitive();
}

#endif


void emitSide(
#ifdef BACKFACE
int i1, int i0
#else
int i0, int i1
#endif
#ifdef DEBUG
,vec3 col
#endif
) {

  vec4 v0 = gl_in[i0].gl_Position;
  vec4 v2 = gl_in[i1].gl_Position;
  
#ifdef POINT
  vec4 v1 = vec4(u_lightPos.w*v0.xyz - u_lightPos.xyz, 0.0);
  vec4 v3 = vec4(u_lightPos.w*v2.xyz - u_lightPos.xyz, 0.0);
#endif

#ifdef DIRECTIONAL
  vec4 v1 = vec4(u_lightDir.xyz, 0.0);
#endif

  //0
  gl_Position = u_projMat*v0;
# ifdef DEBUG
  vertex_color = vec4(col,1.0);
# endif
  EmitVertex();

  //1
  gl_Position = u_projMat*v1;
# ifdef DEBUG
  vertex_color = vec4(col,1.0);
# endif
  EmitVertex();
  
  //2
  gl_Position = u_projMat*v2;
# ifdef DEBUG
  vertex_color = vec4(col,1.0);
# endif
  EmitVertex();
  
  //3
#ifdef POINT
  gl_Position = u_projMat*v3;
# ifdef DEBUG
  vertex_color = vec4(col,1.0);
# endif
  EmitVertex();
#endif

  EndPrimitive();
}

void main() {
  if(!isFaceLit(0,2,4,calcTriNor(0, 2, 4))) {
    return;
  }
  
  for(int i=0; i<3; i++) {
    int i0 = i*2;
    int nb = (i*2+1);
    int i1 = (i*2+2) % 6;
    
    // first vertex with w==0.0 reserved to represent no-adjacent vertex, 
    if(gl_in[nb].gl_Position.w==0.0) {
#ifdef DEBUG
      emitSide(i0,i1,vec3(0.3,0.3,1.0));
#else
      emitSide(i0,i1);
#endif
    } else if(!isFaceLit(i0,nb,i1,calcTriNor(i0,nb,i1))) {
#ifdef DEBUG
      emitSide(i0,i1,vec3(0.2,0.6,0.3));
#else
      emitSide(i0,i1);
#endif
    } 
  }
  
#ifndef ZPASS 

# ifdef DEBUG
  //emitBackCap(0,2,4,vec3(1.0,1.0,0.0));
  emitFrontCap(0,2,4,vec3(1.0,0.0,0.0));
# else
  emitFrontCap(0,2,4);
# endif

# ifdef POINT
#  ifdef DEBUG
  emitBackCap(0,2,4,vec3(1.0,1.0,0.0));
#  else
  emitBackCap(0,2,4);
#  endif
# endif

#endif 
}
