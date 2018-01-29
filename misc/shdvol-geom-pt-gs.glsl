#version 440

/*
point light, backfaced, zfail, infinite
can be converted for front faced and zpass by following notes in comments
expects vertex at index 0 to represent no adjacency (aka a hole in the mesh)
*/

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices=18) out; //12 for zpass

in int vertexID[];

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
    return (dot(n,d0)<0.0 || dot(n,d1)<0.0 || dot(n,d2)<0.0); //for front faced use '>'
}

void main() {
    if(!isFaceLit(0,2,4,calcTriNor(0,2,4))) {
        return;
    }

    for(int i=0; i<3; i++) {
        int i0 = i*2;
        int nb = (i*2+1);
        int i1 = (i*2+2) % 6;

        if(vertexID[nb]==0 || !isFaceLit(i0,nb,i1,calcTriNor(i0,nb,i1))) {

            //emit side quad
            gl_Position = u_projMat*gl_in[i0].gl_Position;
            EmitVertex();

            gl_Position = u_projMat*vec4(u_lightPos.w*v0.xyz - u_lightPos.xyz, 0.0);
            EmitVertex();

            gl_Position = u_projMat*gl_in[i1].gl_Position;
            EmitVertex();

            gl_Position = u_projMat*vec4(u_lightPos.w*v2.xyz - u_lightPos.xyz, 0.0);
            EmitVertex();

            EndPrimitive();
        }
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
    
    //emit back cap
    gl_Position = u_projMat*vec4(u_lightPos.w*gl_in[4].gl_Position.xyz-u_lightPos.xyz,0.0);
    EmitVertex();

    gl_Position = u_projMat*vec4(u_lightPos.w*gl_in[2].gl_Position.xyz-u_lightPos.xyz,0.0);
    EmitVertex();

    gl_Position = u_projMat*vec4(u_lightPos.w*gl_in[0].gl_Position.xyz-u_lightPos.xyz,0.0);
    EmitVertex();

    EndPrimitive();
}
