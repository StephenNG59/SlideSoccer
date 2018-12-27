#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Shader/shader.h>


enum LightType
{
	PointLight: 0,
	DirectionalLight: 1,
	SpotLight: 2,
}


class Light
{

// -- Constructor --

	public:

		Light();

		~Light();


// -- Basic parameters --

	protected:

		LightType type;

		bool isActive = true;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

// -- --

	public:

		void SetAmbient(glm::vec3 ambient);
		void SetDiffuse(glm::vec3 diffuse);
		void SetSpecular(glm::vec3 specular);

		virtual void SetShaderUniforms(Shader shader) = 0;
		

}