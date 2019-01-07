#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec4 ParticleColor;

uniform sampler2D myTexture;


void main()
{
//	FragColor = vec4(TexCoords.xyx, 1.0) * ParticleColor;
	FragColor = texture(myTexture, TexCoords);
//	FragColor = vec4(TexCoords.y, 0, 0, 1.0);
//	FragColor = ParticleColor;
}