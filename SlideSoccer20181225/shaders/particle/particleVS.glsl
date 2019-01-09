#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec4 ParticleColor;

uniform vec3 position;
//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float size = 1;
uniform bool iceMode = false;

uniform vec4 color;

void main()
{
	TexCoords = aTexCoords;
	ParticleColor = color;
	if (iceMode)
	{
		ParticleColor.rgb *= 0.7;
		ParticleColor.b += 0.3 * (ParticleColor.r + ParticleColor.g);
	}

	gl_Position = projection * view * vec4(position + aPos * size, 1.0);
}