#version 330

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices=18) out;

uniform mat4 u_projMat; 
uniform vec4 u_lightPos;

vec3 calcTriNor(int i0, int i1, int i2) {
  vec3 e0=gl_in[i1].gl_Position.xyz - gl_in[i0].gl_Position.xyz;
  vec3 e1=gl_in[i2].gl_Position.xyz - gl_in[i0].gl_Position.xyz;
  vec3 n=cross(e0,e1);
  return n;
}

bool isFaceLit(int i0, int i1, int i2, vec3 n) {
  vec3 d0 = u_lightPos.xyz-u_lightPos.w*gl_in[i0].gl_Position.xyz;
  vec3 d1 = u_lightPos.xyz-u_lightPos.w*gl_in[i1].gl_Position.xyz;
  vec3 d2 = u_lightPos.xyz-u_lightPos.w*gl_in[i2].gl_Position.xyz;

    d0*=-1.0;
    d1*=-1.0;
    d2*=-1.0;

  return (dot(n,d0)>0.0 || dot(n,d1)>0.0 || dot(n,d2)>0.0);
}


void emitFrontCap(int i2,int i1,int i0) {
  vec4 v0=gl_in[i0].gl_Position;
  vec4 v1=gl_in[i1].gl_Position;
  vec4 v2=gl_in[i2].gl_Position;
  
  //0
  gl_Position = u_projMat*v0;
  EmitVertex();

  //1
  gl_Position = u_projMat*v1;
  EmitVertex();

  //2
  gl_Position = u_projMat*v2;
  EmitVertex();
  
  //
  EndPrimitive();
}

void emitBackCap(int i2,int i1,int i0) {  
  vec4 v0=vec4(u_lightPos.w*gl_in[i0].gl_Position.xyz-u_lightPos.xyz,0.0);
  vec4 v1=vec4(u_lightPos.w*gl_in[i1].gl_Position.xyz-u_lightPos.xyz,0.0);
  vec4 v2=vec4(u_lightPos.w*gl_in[i2].gl_Position.xyz-u_lightPos.xyz,0.0);
 
  //2 
  gl_Position = u_projMat*v2;
  EmitVertex();

  //1 
  gl_Position = u_projMat*v1;
  EmitVertex();

  //0 
  gl_Position = u_projMat*v0;
  EmitVertex();

  //
  EndPrimitive();
}

void emitSide(int i1, int i0) {
  vec4 v0 = gl_in[i0].gl_Position;
  vec4 v2 = gl_in[i1].gl_Position;
  
  vec4 v1 = vec4(u_lightPos.w*v0.xyz - u_lightPos.xyz, 0.0);
  vec4 v3 = vec4(u_lightPos.w*v2.xyz - u_lightPos.xyz, 0.0);

  //0
  gl_Position = u_projMat*v0;
  EmitVertex();

  //1
  gl_Position = u_projMat*v1;
  EmitVertex();
  
  //2
  gl_Position = u_projMat*v2;
  EmitVertex();
  
  //3
  gl_Position = u_projMat*v3;
  EmitVertex();

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
    if(gl_in[nb].gl_Position.w==0.0 || !isFaceLit(i0,nb,i1,calcTriNor(i0,nb,i1))) {
      emitSide(i0,i1);
    } 
  }

  //emitFrontCap(0,2,4);
  emitBackCap(0,2,4);
}
