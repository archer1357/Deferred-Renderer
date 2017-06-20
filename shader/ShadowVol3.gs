#version 330

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices=64) out;

uniform mat4 u_projMat; 
uniform vec4 u_lightPos;
out vec4 vertex_color;

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
 

void emitSide(int i1, int nb, int i0) {
  vec4 v0 = gl_in[i0].gl_Position;
  vec4 v1 = gl_in[i1].gl_Position;
  vec4 vnb = gl_in[nb].gl_Position;
  
  vec4 v2 = vec4(u_lightPos.w*v0.xyz - u_lightPos.xyz, 0.0);
  vec4 v3 = vec4(u_lightPos.w*v1.xyz - u_lightPos.xyz, 0.0);
  
  vec4 v4 = vec4(u_lightPos.w*(v0.xyz+(v0.xyz-vnb.xyz)*1.5) - u_lightPos.xyz, 0.0);
  vec4 v5 = vec4(u_lightPos.w*(v1.xyz+(v1.xyz-vnb.xyz)*1.5) - u_lightPos.xyz, 0.0);
/*
  //0
  gl_Position = u_projMat*v0;
  vertex_color=vec4(0.8,0.8,0.8,1.0);
  EmitVertex();

  //1
  gl_Position = u_projMat*v2;
  vertex_color=vec4(0.8,0.8,0.8,1.0);
  EmitVertex();
  
  //2
  gl_Position = u_projMat*v1;
  vertex_color=vec4(0.8,0.8,0.8,1.0);
  EmitVertex();
  
  //3
  gl_Position = u_projMat*v3;
  vertex_color=vec4(0.8,0.8,0.8,1.0);
  EmitVertex();

  EndPrimitive();
  */
  //
  //0
  gl_Position = u_projMat*v0;
  vertex_color=vec4(1.0,0.3,0.4,1.0);
  EmitVertex();

  //1
  gl_Position = u_projMat*v4;
  vertex_color=vec4(1.0,0.3,0.4,1.0);
  EmitVertex();
  
  //2
  gl_Position = u_projMat*v1;
  vertex_color=vec4(1.0,0.3,0.4,1.0);
  EmitVertex();
  
  //3
  gl_Position = u_projMat*v5;
  vertex_color=vec4(1.0,0.3,0.4,1.0);
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
      emitSide(i0,nb,i1);
    } 
  }
}
