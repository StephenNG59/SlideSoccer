#include "stdafx.h"
#include "PointLight.h"


void PointLight::SetShaderUniforms(Shader shader)
{
	
	shader.setVec3("pointLight0.position", position);

	shader.setVec3("pointLight0.ambient", ambient);
	shader.setVec3("pointLight0.diffuse", diffuse);
	shader.setVec3("pointLight0.specular", specular);

	shader.setFloat("pointLight0.constant", constant);
	shader.setFloat("pointLight0.linear", linear);
	shader.setFloat("pointLight0.quadratic", quadratic);

}