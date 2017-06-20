
layout(triangles_adjacency) in;

layout(line_strip, max_vertices=18) out;


uniform mat4 u_projMat;
uniform vec4 u_lightPos;

uniform bool u_robust;  // Robust generation needed?
uniform bool u_zpass;  // Is it safe to do z-pass?


bool bla(int i0, int i1, int i2) {

  vec3 ns[3];  // Normals
  vec3 d[3];  // Directions toward light

  // Compute normal at each vertex.
  ns[0] = cross(gl_in[i1].gl_Position.xyz - gl_in[i0].gl_Position.xyz,
                gl_in[i2].gl_Position.xyz - gl_in[i0].gl_Position.xyz );
  ns[1] = cross(gl_in[i2].gl_Position.xyz - gl_in[i1].gl_Position.xyz,
                gl_in[i0].gl_Position.xyz - gl_in[i1].gl_Position.xyz );
  ns[2] = cross(gl_in[i0].gl_Position.xyz - gl_in[i2].gl_Position.xyz,
                gl_in[i1].gl_Position.xyz - gl_in[i2].gl_Position.xyz );

  // Compute direction from vertices to light.
  d[0] = u_lightPos.xyz-u_lightPos.w*gl_in[i0].gl_Position.xyz;
  d[1] = u_lightPos.xyz-u_lightPos.w*gl_in[i1].gl_Position.xyz;
  d[2] = u_lightPos.xyz-u_lightPos.w*gl_in[i2].gl_Position.xyz;

  return (dot(ns[0],d[0])>0.0 || dot(ns[1],d[1])>0.0 || dot(ns[2],d[2])>0.0);
}

void main() {
  vec4 v[4];  // Temporary vertices

  //Triangle oriented toward light source
  vec4 or_pos[3] = {
    gl_in[0].gl_Position,
    gl_in[2].gl_Position,
    gl_in[4].gl_Position
  };

  // Check if the main triangle faces the light.
  bool faces_light = true;

  if(!bla(0,2,4)) {
    // Not facing the light and not robust, ignore.
    if(!u_robust) {
      return;
    }

    // Flip vertex winding order in or_pos.
    or_pos[1] = gl_in[4].gl_Position;
    or_pos[2] = gl_in[2].gl_Position;
    faces_light = false;
  }

  // Render caps. This is only needed for z-fail.
  if(!u_zpass) {
    // Near cap: simply render triangle.
    gl_Position = u_projMat*or_pos[0];

    EmitVertex();
    gl_Position = u_projMat*or_pos[1];

    EmitVertex();
    gl_Position = u_projMat*or_pos[2];

    EmitVertex();
    EndPrimitive();

    // Far cap: extrude positions to infinity.
    v[0] =vec4(u_lightPos.w*or_pos[0].xyz-u_lightPos.xyz,0);
    v[1] =vec4(u_lightPos.w*or_pos[2].xyz-u_lightPos.xyz,0);
    v[2] =vec4(u_lightPos.w*or_pos[1].xyz-u_lightPos.xyz,0);
    gl_Position = u_projMat*v[0];

    EmitVertex();
    gl_Position = u_projMat*v[1];

    EmitVertex();
    gl_Position = u_projMat*v[2];

    EmitVertex();
    EndPrimitive();
  }

  // Loop over all edges and extrude if needed.
  for(int i=0; i<3; i++) {
    // Compute indices of neighbor triangle.
    int v0 = i*2;
    int nb = (i*2+1);
    int v1 = (i*2+2) % 6;

    // Extrude the edge if it does not have a
    // neighbor, or if it's a possible silhouette.
    if(gl_in[nb].gl_Position.w < 1e-3 || faces_light != bla(v0,nb,v1)) {
        // Make sure sides are oriented correctly.
        int i0 = faces_light ? v0 : v1;
        int i1 = faces_light ? v1 : v0;

        v[0] = gl_in[i0].gl_Position;
        v[1] = vec4(u_lightPos.w*gl_in[i0].gl_Position.xyz - u_lightPos.xyz, 0.0);
        v[2] = gl_in[i1].gl_Position;
        v[3] = vec4(u_lightPos.w*gl_in[i1].gl_Position.xyz - u_lightPos.xyz, 0.0);

        // Emit a quad as a triangle strip.
        gl_Position = u_projMat*v[0];

        EmitVertex();
        gl_Position = u_projMat*v[1];

        EmitVertex();
        gl_Position = u_projMat*v[2];

        EmitVertex();
        gl_Position = u_projMat*v[3];

        EmitVertex();
        EndPrimitive();
      }
  }
}
