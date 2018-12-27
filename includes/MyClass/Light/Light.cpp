#include "stdafx.h"
#include "Light.h"

void Light::SetAmbient(glm::vec3 ambient)
{
	this->ambient = ambient;
}

void Light::SetDiffuse(glm::vec3 diffuse)
{
	this->diffuse = diffuse;
}

void Light::SetSpecular(glm::vec3 specular)
{
	this->specular = specular;
}