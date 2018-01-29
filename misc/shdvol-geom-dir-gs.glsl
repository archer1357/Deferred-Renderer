#version 440

/*
directional light, backfaced, zfail, infinite
can be converted for front faced and zpass by following notes in comments
expects vertex at index 0 to represent no adjacency (aka a hole in the mesh)
*/

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices=12) out; //9 for zpass

in int vertexID[];

uniform mat4 u_projMat;
uniform vec4 u_lightDir;

vec3 calcTriNor(int i0, int i1, int i2) {
  vec3 e0=gl_in[i1].gl_Position.xyz - gl_in[i0].gl_Position.xyz;
  vec3 e1=gl_in[i2].gl_Position.xyz - gl_in[i0].gl_Position.xyz;
  vec3 n=cross(e0,e1);
  return n;
}

bool isFaceLit(int i0, int i1, int i2, vec3 n) {
  vec3 d=u_lightDir.xyz;
  return (dot(n,d)>0.0); //for front faced use '<'
}

void main() {
  if(!isFaceLit(0,2,4,calcTriNor(0, 2, 4))) {
    return;
  }

  for(int i=0; i<3; i++) {
    int i0 = i*2;
    int nb = (i*2+1);
    int i1 = (i*2+2) % 6;

    if(vertexID[nb]==0 || !isFaceLit(i0,nb,i1,calcTriNor(i0,nb,i1))) {
      //emit side
      gl_Position = u_projMat*gl_in[i0].gl_Position;
      EmitVertex();

      gl_Position = u_projMat*vec4(u_lightDir.xyz, 0.0);
      EmitVertex();

      gl_Position = u_projMat*gl_in[i1].gl_Position;
      EmitVertex();

      EndPrimitive();
  }

  //for zpass remove code below

  //emit front cap
  gl_Position = u_projMat*gl_in[0].gl_Position;
  EmitVertex();

  gl_Position = u_projMat*gl_in[2].gl_Position;
  EmitVertex();

  gl_Position = u_projMat*gl_in[4].gl_Position;
  EmitVertex();

  EndPrimitive();
}
