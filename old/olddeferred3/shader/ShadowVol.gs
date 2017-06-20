layout(triangles_adjacency) in;

#ifdef POINT
#ifdef DEBUG
//layout(line_strip, max_vertices=12) out;
layout(line_strip, max_vertices=18) out;
#else
layout(triangle_strip, max_vertices=18) out;
#endif
#endif

#ifdef DIRECTIONAL
#ifdef DEBUG
layout(line_strip, max_vertices=12) out;
#else
layout(triangle_strip, max_vertices=12) out;
#endif
#endif

//in Vertex {
//  vec3 normal;
//} vertex[];

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

uniform bool u_zPass;
uniform bool u_backfaced;

vec3 calcTriNor(int i0, int i1, int i2) {
  vec3 e0=gl_in[i1].gl_Position.xyz - gl_in[i0].gl_Position.xyz;
  vec3 e1=gl_in[i2].gl_Position.xyz - gl_in[i0].gl_Position.xyz;
  vec3 n=cross(e0,e1);
  //n=normalize(n);
  return n;
}

bool isFaceLit(int i0, int i1, int i2, vec3 n) {
#ifdef POINT
  // Compute direction from vertices to light.
  vec3 d0 = u_lightPos.xyz-u_lightPos.w*gl_in[i0].gl_Position.xyz;
  vec3 d1 = u_lightPos.xyz-u_lightPos.w*gl_in[i1].gl_Position.xyz;
  vec3 d2 = u_lightPos.xyz-u_lightPos.w*gl_in[i2].gl_Position.xyz;

  if(u_backfaced) {
    d0*=-1.0;
    d1*=-1.0;
    d2*=-1.0;
  }

  return (dot(n,d0)>0.0 || dot(n,d1)>0.0 || dot(n,d2)>0.0);
#endif

#ifdef DIRECTIONAL
  vec3 d=u_lightDir.xyz;

  if(!u_backfaced) {
    d*=-1.0;
  }

  return (dot(n,d)>0.0);
#endif
}

void emitCaps() {

//#ifdef DEBUG
//return;
//#endif
  if(u_backfaced) {
    gl_Position = u_projMat*gl_in[4].gl_Position;
#ifdef DEBUG
    vertex_color = vec4(1.0,0.0,1.0,1.0);
    // vertex_color=abs(normalize(gl_in[4].gl_Position));
#endif
    EmitVertex();

    gl_Position = u_projMat*gl_in[2].gl_Position;
#ifdef DEBUG
    vertex_color = vec4(1.0,0.0,1.0,1.0);
    // vertex_color=abs(normalize(gl_in[2].gl_Position));
#endif
    EmitVertex();

    gl_Position = u_projMat*gl_in[0].gl_Position;
#ifdef DEBUG
    vertex_color = vec4(1.0,0.0,1.0,1.0);
    // vertex_color=abs(normalize(gl_in[0].gl_Position));
#endif
    EmitVertex();
  } else {
    gl_Position = u_projMat*gl_in[0].gl_Position;
#ifdef DEBUG
    vertex_color = vec4(1.0,0.0,1.0,1.0);
    // vertex_color=abs(normalize(gl_in[0].gl_Position));
#endif
    EmitVertex();

    gl_Position = u_projMat*gl_in[2].gl_Position;
#ifdef DEBUG
    vertex_color = vec4(1.0,0.0,1.0,1.0);
    // vertex_color=abs(normalize(gl_in[2].gl_Position));
#endif
    EmitVertex();

    gl_Position = u_projMat*gl_in[4].gl_Position;
#ifdef DEBUG
    vertex_color = vec4(1.0,0.0,1.0,1.0);
    // vertex_color=abs(normalize(gl_in[4].gl_Position));
#endif
    EmitVertex();
  }

  EndPrimitive();

#ifdef POINT
  vec4 v0=vec4(u_lightPos.w*gl_in[0].gl_Position.xyz-u_lightPos.xyz,0.0);
  vec4 v1=vec4(u_lightPos.w*gl_in[4].gl_Position.xyz-u_lightPos.xyz,0.0);
  vec4 v2=vec4(u_lightPos.w*gl_in[2].gl_Position.xyz-u_lightPos.xyz,0.0);

  if(u_backfaced) {
    gl_Position = u_projMat*v2;
#ifdef DEBUG
    vertex_color = vec4(1.0,1.0,0.0,1.0);
    // vertex_color=abs(normalize(v2));
#endif
    EmitVertex();

    gl_Position = u_projMat*v1;
#ifdef DEBUG
    vertex_color = vec4(1.0,1.0,0.0,1.0);
    // vertex_color=abs(normalize(v1));
#endif
    EmitVertex();

    gl_Position = u_projMat*v0;
#ifdef DEBUG
    vertex_color = vec4(1.0,1.0,0.0,1.0);
    // vertex_color=abs(normalize(v0));
#endif
    EmitVertex();
  } else {
    gl_Position = u_projMat*v0;
#ifdef DEBUG
    vertex_color = vec4(1.0,1.0,0.0,1.0);
    // vertex_color=abs(normalize(v0));
#endif
    EmitVertex();

    gl_Position = u_projMat*v1;
#ifdef DEBUG
    vertex_color = vec4(1.0,1.0,0.0,1.0);
    // vertex_color=abs(normalize(v1));
#endif
    EmitVertex();

    gl_Position = u_projMat*v2;
#ifdef DEBUG
    vertex_color = vec4(1.0,1.0,0.0,1.0);
    // vertex_color=abs(normalize(v2));
#endif
    EmitVertex();
  }

  EndPrimitive();
#endif
}

void emitSide(int i0, int i1
#ifdef DEBUG
,vec3 col
#endif
) {
#ifdef POINT
  vec4 v0 = gl_in[i0].gl_Position;
  vec4 v1 = vec4(u_lightPos.w*gl_in[i0].gl_Position.xyz - u_lightPos.xyz, 0.0);
  vec4 v2 = gl_in[i1].gl_Position;
  vec4 v3 = vec4(u_lightPos.w*gl_in[i1].gl_Position.xyz - u_lightPos.xyz, 0.0);

  if(u_backfaced) {
    gl_Position = u_projMat*v1;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
    // vertex_color=abs(normalize(v1));
#endif
    EmitVertex();

    gl_Position = u_projMat*v0;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
    // vertex_color=abs(normalize(v0));
#endif
    EmitVertex();

    gl_Position = u_projMat*v3;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
    // vertex_color=abs(normalize(v3));
#endif
    EmitVertex();

    gl_Position = u_projMat*v2;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
    // vertex_color=abs(normalize(v2));
#endif
    EmitVertex();
  } else {
    gl_Position = u_projMat*v0;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
    // vertex_color=abs(normalize(v0));
#endif
    EmitVertex();

    gl_Position = u_projMat*v1;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
    // vertex_color=abs(normalize(v1));
#endif
    EmitVertex();

    gl_Position = u_projMat*v2;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
    // vertex_color=abs(normalize(v2));
#endif
    EmitVertex();

    gl_Position = u_projMat*v3;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
    // vertex_color=abs(normalize(v3));
#endif
    EmitVertex();
  }
#endif

#ifdef DIRECTIONAL
  vec4 v0 = gl_in[i0].gl_Position;
  vec4 v1 = vec4(u_lightDir.xyz, 0.0);
  vec4 v2 = gl_in[i1].gl_Position;

  if(u_backfaced) {
    gl_Position = u_projMat*v2;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
#endif
    EmitVertex();

    gl_Position = u_projMat*v1;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
#endif
    EmitVertex();

    gl_Position = u_projMat*v0;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
#endif
    EmitVertex();
  } else {
    gl_Position = u_projMat*v0;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
#endif
    EmitVertex();

    gl_Position = u_projMat*v1;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
#endif
    EmitVertex();

    gl_Position = u_projMat*v2;
#ifdef DEBUG
    vertex_color = vec4(col,1.0);
#endif
    EmitVertex();
  }
#endif

  EndPrimitive();
}

void main() {
  vec3 mainNor=calcTriNor(0, 2, 4);
  float mainD=dot(mainNor,gl_in[0].gl_Position.xyz)*-1.0;

//mainNor=cross(gl_PositionIn[2].xyz-gl_PositionIn[0].xyz,gl_PositionIn[4].xyz-gl_PositionIn[0].xyz);
//mainNor=cross(gl_PositionIn[4].xyz-gl_PositionIn[2].xyz,gl_PositionIn[0].xyz-gl_PositionIn[2].xyz);
//mainNor=cross(gl_PositionIn[0].xyz-gl_PositionIn[4].xyz,gl_PositionIn[2].xyz-gl_PositionIn[4].xyz);
  if(!isFaceLit(0,2,4,mainNor)) {
    return;
  }

  if(!u_zPass) {

    emitCaps();

  }

  for(int i=0; i<3; i++) {
    int i0 = i*2;
    int nb = (i*2+1);
    int i1 = (i*2+2) % 6;

    if(gl_in[nb].gl_Position.w==0.0) { //first vertex reserved to represent no-adjacent vertex, with w==0.0
      emitSide(i0,i1
#ifdef DEBUG
,vec3(0.3,0.3,1.0)
#endif
      );
    } else {
      vec3 otherNor=calcTriNor(i0,nb,i1);

      if(isFaceLit(i0,nb,i1,otherNor)) {
        //vec3 n=normalize(mainNor);
        //float pd=dot(n,gl_in[i0].gl_Position.xyz);


        //int j=(2*(i+4)+2) % 6;
        //vec3 e=gl_in[j].gl_Position.xyz-gl_in[i0].gl_Position.xyz;
        //vec3 e=gl_in[i0].gl_Position.xyz-gl_in[j].gl_Position.xyz;
        //e=normalize(e);

      //  vec3 e=gl_in[i0].gl_Position.xyz-gl_in[nb].gl_Position.xyz;
        vec3 e=gl_in[nb].gl_Position.xyz-gl_in[i0].gl_Position.xyz;

        float d=dot(normalize(mainNor),normalize(e));



        // if(d > 1111112.0)
        // if(dot(mainNor,gl_in[nb].gl_Position.xyz) + mainD > 0.01)
        if(false)
        // if(d>0.0)
        {

      emitSide(i0,i1
#ifdef DEBUG
,vec3(1.0,0.0,0.0)
#endif
      );
        }
      } else {
      emitSide(i0,i1
#ifdef DEBUG
,vec3(0.2,0.6,0.3)
#endif
      );
      }
    }
  }
}
