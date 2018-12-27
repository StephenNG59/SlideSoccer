#pragma once

#include "Light.h"

class PointLight : light
{

// -- Constructor --

	public:

		PointLight(glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f)/*, int distFar*/) : position(position), ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic)
		{
			// for test
			float constant = 1.0f;
			float linear = 0.09f;
			float quadratic = 0.032f;
		}

		~PointLight() {}


// -- --

	protected:

		glm::vec3 position;


	public:

		void SetAmbient(glm::vec3 ambient);
		void SetDiffuse(glm::vec3 diffuse);
		void SetSpecular(glm::vec3 specular);

		void SetShaderUniforms(Shader shader);

}