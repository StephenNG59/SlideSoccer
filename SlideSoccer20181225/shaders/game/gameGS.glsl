#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;
//layout (points, max_vertices = 1) out;

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
void deal_vertex_center();
vec4 getCenterPoint();

vec3 normal;
vec3 point_center[3];

uniform float currentTime;
uniform bool isExploding = false;
uniform float explosionOffset;
uniform float v = 5, a = -5, t;

void main() {   
	
	normal = GetNormal();

	if (!isExploding)				// not exploding
	{
		for (int i = 0; i < 3; i++)
		{
			deal_vertex(i);
			EmitVertex();
		}
	} 
	else if (isExploding)			// exploding
	{
//		deal_vertex_center();
		vec4 center = getCenterPoint();
//		gl_PointSize = 5.0f;
//		gl_Position += (v * t + 0.5f * a * t * t) * normal;
//		EmitVertex();
		vec3 offset =  (v * t + 0.5f * a * t * t) * normal;

		for (int i = 0; i < 3; i++)
		{
			deal_vertex(i);
			gl_Position = (gl_Position + 2 * center) / 3.0f;
			gl_Position += offset;
			EmitVertex();
		}
	}

	EndPrimitive();
}

vec4 getCenterPoint()
{
	vec4 c = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0f;
	return c;
}

void deal_vertex(int index)
{
	gs_out.FragPos = gs_in[index].FragPos;
	gs_out.Normal = gs_in[index].Normal;
	gs_out.TexCoords = gs_in[index].TexCoords;
	gs_out.FragPosLightSpace = gs_in[index].FragPosLightSpace;

	gl_Position = gl_in[index].gl_Position;
//	if (!isExploding)
//		return;

//	gl_Position += explosionOffset * normal;
}

void deal_vertex_center()
{
	gs_out.FragPos = (gs_in[0].FragPos + gs_in[1].FragPos + gs_in[2].FragPos) / 3.0f;
	gs_out.Normal = (gs_in[0].Normal + gs_in[1].Normal + gs_in[2].Normal) / 3.0f;
	gs_out.TexCoords = (gs_in[0].TexCoords + gs_in[1].TexCoords + gs_in[2].TexCoords) / 3.0f;
	gs_out.FragPosLightSpace = (gs_in[0].FragPosLightSpace + gs_in[1].FragPosLightSpace + gs_in[2].FragPosLightSpace) / 3.0f;
	gl_Position = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0f;
}


vec3 GetNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}
