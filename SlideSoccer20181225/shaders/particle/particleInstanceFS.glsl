#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec4 ParticleColor;
in float Explosion;

uniform sampler2D myTexture;
uniform bool iceMode = false;
uniform bool useTexture = true;

void main()
{
	if (Explosion == 1.0f)
	{
		if (useTexture)
			FragColor = ParticleColor * texture(myTexture, TexCoords);
//			FragColor = texture(myTexture, TexCoords);
		else
//			FragColor = ParticleColor * texture(myTexture, TexCoords);
			FragColor = ParticleColor;
//			FragColor = texture(myTexture, TexCoords);
	}
	else
	{
		FragColor = texture(myTexture, TexCoords);
	}
	
	if (iceMode)
	{
		FragColor.rgb *= 0.33;
		FragColor.b += FragColor.g + FragColor.r;
	}
		
}