
layout(triangles) in;
layout(line_strip, max_vertices=20) out;

in Vertex {
  vec3 normal;
  vec2 texcoord;
  vec4 tangent;
} vertex[];


out vec4 vertex_color;

uniform mat4 u_modelViewProjMat;
uniform mat4 u_modelViewMat;
uniform mat4 u_normalMat;
uniform mat4 u_projMat;

void main() {
float q=0.1;
float normal_length=0.3;
vec3 V0=gl_in[2].gl_Position.xyz-gl_in[0].gl_Position.xyz;
vec3 V1=gl_in[2].gl_Position.xyz-gl_in[1].gl_Position.xyz;
vec3 N2=cross(V0,V1);
N2=normalize(N2);
	vec3 P2=(gl_in[0].gl_Position.xyz+gl_in[1].gl_Position.xyz+gl_in[2].gl_Position.xyz)/3.0;
	
    gl_Position = u_modelViewProjMat * vec4(P2, 1.0);
    vertex_color = vec4(1.0,1.0,0.0,1.0);
    EmitVertex();
    
    gl_Position = u_modelViewProjMat * vec4(P2+ N2 * normal_length, 1.0);
    vertex_color = vec4(1.0,1.0,0.0,1.0);
    EmitVertex();
    EndPrimitive();
	
  for(int i=0; i<gl_in.length(); i++) {
  vec4 col=vec4(1.0,1.0,1.0,1.0);
    vec3 P = gl_in[i].gl_Position.xyz;
    vec3 N = vertex[i].normal;
	
	
	vec3 T=vertex[i].tangent.xyz;
  vec3 B=normalize(-cross(N, T)*vertex[i].tangent.w);
   
	//normals
    gl_Position = u_modelViewProjMat * vec4(P+N*q, 1.0);
    vertex_color = vec4(0.0,1.0,0.0,1.0);
    EmitVertex();
    
    gl_Position = u_modelViewProjMat * vec4(P+N*q + N * normal_length, 1.0);
    vertex_color = vec4(0.0,1.0,0.0,1.0);
    EmitVertex();
    EndPrimitive();
 
    //tangents 
    gl_Position = u_modelViewProjMat * vec4(P+N*q, 1.0);
    vertex_color = vec4(1.0,0.0,0.0,1.0);
    EmitVertex();
    
    gl_Position = u_modelViewProjMat * vec4(P+N*q + T * normal_length, 1.0);
    vertex_color = vec4(1.0,0.0,0.0,1.0);
    EmitVertex();
    EndPrimitive();

	//bitangents
    gl_Position = u_modelViewProjMat * vec4(P+N*q, 1.0);
    vertex_color = vec4(0.0,0.0,1.0,1.0);
    EmitVertex();
    
    gl_Position = u_modelViewProjMat * vec4(P+N*q + B * normal_length, 1.0);
    vertex_color = vec4(0.0,0.0,1.0,1.0);
    EmitVertex();
    EndPrimitive();

	
  }
}