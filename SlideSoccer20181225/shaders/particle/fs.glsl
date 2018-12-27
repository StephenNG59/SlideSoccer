#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec4 ParticleColor;

void main()
{
//	FragColor = vec4(TexCoords.xyx, 1.0) * ParticleColor;
	FragColor = vec4(0.8, 0.2, 0.3, 1.0) * ParticleColor;
}