#version 330 core

layout (location = 0) in vec3 aPos;		// vertices (the same for all particles)
layout (location = 1) in vec4 xyzl;		// positions and life
layout (location = 2) in vec4 color;	// colors

//out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	ParticleColor =color * xyzl.w;
	gl_Position = projection * view * vec4(xyzl.xyz + aPos, 1.0);
}