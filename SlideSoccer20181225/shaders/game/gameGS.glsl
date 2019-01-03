#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_GS {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} gs_in[];

out GS_FS {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} gs_out;

vec3 GetNormal();
void deal_vertex(int index);
void explode_1();

vec3 normal;

uniform float currentTime;
uniform bool isExploding = false;
uniform float explosionOffset;

void main() {   
	
	normal = GetNormal();
	for (int i = 0; i < 3; i++)
	{
		deal_vertex(i);
		EmitVertex();
	}
	EndPrimitive();
}


void deal_vertex(int index)
{
	gs_out.FragPos = gs_in[index].FragPos;
	gs_out.Normal = gs_in[index].Normal;
	gs_out.TexCoords = gs_in[index].TexCoords;
	gs_out.FragPosLightSpace = gs_in[index].FragPosLightSpace;

	gl_Position = gl_in[index].gl_Position;
	if (!isExploding)
		return;

	gl_Position += explosionOffset * normal;
}


vec3 GetNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}
