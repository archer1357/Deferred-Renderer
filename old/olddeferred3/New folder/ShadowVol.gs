

layout(triangles_adjacency) in;


#ifdef POINT
#ifdef DEBUG
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

#ifdef SMOOTH
in Vertex {
  vec3 normal;
} vertex[];

uniform float u_smoothBias;
#endif

uniform mat4 u_projMat;

#ifdef POINT
uniform vec4 u_lightPos;  // Light position (eye space)
#endif

#ifdef DIRECTIONAL
uniform vec4 u_lightDir;  // Light position (eye space)
#endif

uniform bool u_robust;  // Robust generation needed?
uniform bool u_zPass;  // Is it safe to do z-pass?
uniform bool u_backfaced;

bool bla(int i0, int i1, int i2) {

  vec3 ns[3];  // Normals

  // Compute normal at each vertex.

  ns[0] = cross(gl_in[i1].gl_Position.xyz - gl_in[i0].gl_Position.xyz,
                gl_in[i2].gl_Position.xyz - gl_in[i0].gl_Position.xyz );
  ns[1] = cross(gl_in[i2].gl_Position.xyz - gl_in[i1].gl_Position.xyz,
                gl_in[i0].gl_Position.xyz - gl_in[i1].gl_Position.xyz );
  ns[2] = cross(gl_in[i0].gl_Position.xyz - gl_in[i2].gl_Position.xyz,
                gl_in[i1].gl_Position.xyz - gl_in[i2].gl_Position.xyz );

  //ns[0]=vertex[i0].normal;
  //ns[1]=vertex[i1].normal;
  //ns[2]=vertex[i2].normal;

  // Compute direction from vertices to light.
#ifdef POINT
  vec3 d[3];  // Directions toward light
  d[0] = u_lightPos.xyz-u_lightPos.w*gl_in[i0].gl_Position.xyz;
  d[1] = u_lightPos.xyz-u_lightPos.w*gl_in[i1].gl_Position.xyz;
  d[2] = u_lightPos.xyz-u_lightPos.w*gl_in[i2].gl_Position.xyz;

  if(u_backfaced) {
    for(int i=0;i<3;i++) {
      d[i]*=-1.0;
    }
  }
  return (dot(ns[0],d[0])>0.0 || dot(ns[1],d[1])>0.0 || dot(ns[2],d[2])>0.0);
#endif

#ifdef DIRECTIONAL
  vec3 d=u_lightDir.xyz;

  if(!u_backfaced) {
    d*=-1.0;
  }

  return (dot(ns[0],d)>0.0 || dot(ns[1],d)>0.0 || dot(ns[2],d)>0.0);
#endif
}

void main() {
#ifdef SMOOTH
  float smoothBias=-u_smoothBias*0.1;
#endif
  vec4 v[4];  // Temporary vertices

  //Triangle oriented toward light source
  vec4 or_pos[3] = {
    gl_in[0].gl_Position,
    gl_in[2].gl_Position,
    gl_in[4].gl_Position
  };



  // Check if the main triangle faces the light.
  bool faces_light = true;
  bool waaa=bla(0,2,4);
#if defined SMOOTH && !defined DEBUG
  or_pos[0]+=vec4(vertex[0].normal,0.0)*smoothBias;
  or_pos[1]+=vec4(vertex[2].normal,0.0)*smoothBias;
  or_pos[2]+=vec4(vertex[4].normal,0.0)*smoothBias;
#endif
  if(!waaa) {
    // Not facing the light and not robust, ignore.
    if(!u_robust) {
      return;
    }

    // Flip vertex winding order in or_pos.
    vec4 tmp=or_pos[1];
    or_pos[1]=or_pos[2];
    or_pos[2]=tmp;

    //or_pos[1] = gl_in[4].gl_Position;
    //or_pos[2] = gl_in[2].gl_Position;
    faces_light = false;
  }

  // Render caps. This is only needed for z-fail.

  if(!u_zPass) {
    if(waaa) { //
       if(u_backfaced) {
        gl_Position = u_projMat*or_pos[2];
        EmitVertex();

        gl_Position = u_projMat*or_pos[1];
        EmitVertex();

        gl_Position = u_projMat*or_pos[0];
        EmitVertex();
       } else {
        // Near cap: simply render triangle.
        gl_Position = u_projMat*or_pos[0];
        EmitVertex();

        gl_Position = u_projMat*or_pos[1];
        EmitVertex();

        gl_Position = u_projMat*or_pos[2];
        EmitVertex();
       }
      EndPrimitive();

#ifdef POINT
      // Far cap: extrude positions to infinity.
      v[0] =vec4(u_lightPos.w*or_pos[0].xyz-u_lightPos.xyz,0.0);
      v[1] =vec4(u_lightPos.w*or_pos[2].xyz-u_lightPos.xyz,0.0);
      v[2] =vec4(u_lightPos.w*or_pos[1].xyz-u_lightPos.xyz,0.0);

       if(u_backfaced) {
        gl_Position = u_projMat*v[2];
        EmitVertex();
        
        gl_Position = u_projMat*v[1];
        EmitVertex();

        gl_Position = u_projMat*v[0];
        EmitVertex();
        
      
        
       } else {
        gl_Position = u_projMat*v[0];
        EmitVertex();

        gl_Position = u_projMat*v[1];
        EmitVertex();

        gl_Position = u_projMat*v[2];
        EmitVertex();
       }
      
      EndPrimitive();
#endif
    }
  }

  // Loop over all edges and extrude if needed.
  for(int i=0; i<3; i++) {
    // Compute indices of neighbor triangle.
    int v0 = i*2;
    int nb = (i*2+1);
    int v1 = (i*2+2) % 6;

    // Extrude the edge if it does not have a
    // neighbor, or if it's a possible silhouette.
    if(gl_in[nb].gl_Position.w==0.0 || faces_light != bla(v0,nb,v1)) { // < 1e-3
      // Make sure sides are oriented correctly.
      int i0 = faces_light ? v0 : v1;
      int i1 = faces_light ? v1 : v0;
#ifdef POINT
      v[0] = gl_in[i0].gl_Position;
      v[1] = vec4(u_lightPos.w*gl_in[i0].gl_Position.xyz - u_lightPos.xyz, 0.0);
      v[2] = gl_in[i1].gl_Position;
      v[3] = vec4(u_lightPos.w*gl_in[i1].gl_Position.xyz - u_lightPos.xyz, 0.0);

#if defined SMOOTH //&& !defined DEBUG
      v[0]+=vec4(vertex[i0].normal,0.0)*smoothBias;
      v[2]+=vec4(vertex[i1].normal,0.0)*smoothBias;
#endif

      if(u_backfaced) {
        gl_Position = u_projMat*v[1];
        EmitVertex();

        gl_Position = u_projMat*v[0];
        EmitVertex();
        
        
        gl_Position = u_projMat*v[3];
        EmitVertex();

        gl_Position = u_projMat*v[2];
        EmitVertex();
      } else {
        gl_Position = u_projMat*v[0];
        EmitVertex();

        gl_Position = u_projMat*v[1];
        EmitVertex();

        gl_Position = u_projMat*v[2];
        EmitVertex();

        gl_Position = u_projMat*v[3];
        EmitVertex();
      }
#endif

#ifdef DIRECTIONAL
      v[0] = gl_in[i0].gl_Position;
      v[1] = vec4(u_lightDir.xyz, 0.0);
      v[2] = gl_in[i1].gl_Position;

#if defined SMOOTH //&& !defined DEBUG
      v[0].xyz+=vertex[i0].normal*smoothBias;
      v[2].xyz+=vertex[i1].normal*smoothBias;
#endif

      if(u_backfaced) {
        gl_Position = u_projMat*v[2];
        EmitVertex();

        gl_Position = u_projMat*v[1];
        EmitVertex();

        gl_Position = u_projMat*v[0];
        EmitVertex();
      } else {
        gl_Position = u_projMat*v[0];
        EmitVertex();

        gl_Position = u_projMat*v[1];
        EmitVertex();

        gl_Position = u_projMat*v[2];
        EmitVertex();
      }

#endif

      EndPrimitive();
    }
  }
}
