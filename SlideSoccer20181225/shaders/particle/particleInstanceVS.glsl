#version 330 core

layout (location = 0) in vec3 aPos;		// vertices (the same for all particles)
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 xyzl;		// positions and life
layout (location = 3) in vec4 color;	// colors
layout (location = 4) in float size;

out vec2 TexCoords;
out vec4 ParticleColor;
out float Explosion;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform int lineNum;
uniform int columnNum;
uniform float isExplosion = 0.0f;

uniform float maxLife;

vec2 calcTexCoords();

void main()
{
	ParticleColor = color/* * xyzl.w*/;
//	ParticleColor = vec4(0.5, 0.5, 0.7, 1.0);
	TexCoords = calcTexCoords();
	Explosion = isExplosion;

	vec3 vertexPosition_worldspace = xyzl.xyz + size * cameraRight * aPos.x + size * cameraUp * aPos.y;
	gl_Position = projection * view * vec4(vertexPosition_worldspace, 1.0);

//	gl_Position = projection * view * vec4(xyzl.xyz + aPos, 1.0);
}

vec2 calcTexCoords()
{
	float age = maxLife - xyzl.w;
	int index = int(age / maxLife * lineNum * columnNum);
	int line = lineNum - 1 - index / lineNum, column = index % columnNum;
	float lineHeight = 1.0f / lineNum, columnWidth = 1.0f / columnNum;

	float u = (column + aTexCoords.x) * columnWidth;
	float v = (line + aTexCoords.y) * lineHeight;

//	return vec2(0.5 + TexCoords.x, 0.5 + TexCoords.y);
	return vec2(u, v);
}