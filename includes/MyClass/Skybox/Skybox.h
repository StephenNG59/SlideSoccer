#pragma once

#include <vector>
#include <string>

#include <stb_image.h>
#include <MyClass/Shader/shader.h>
#include <MyClass/camera.h>

class Skybox
{

	public:
		
		// Constructor
		Skybox(std::vector<std::string> facesPath, std::vector<std::string> shadersPath);
		~Skybox();

		// Render function
		void Draw(Camera camera);

		unsigned int CubemapTexture;

	private:

		// Shader
		Shader *skyboxShader;

		// Render state
		unsigned int VAO;

		// Initializes buffer and vertex attributes
		void init(std::vector<std::string> facesPath, std::vector<std::string> shadersPath);

};

// loads a cubemap texture from 6 individual texture faces
// -------------------------------------------------------
unsigned int loadCubemap(std::vector<std::string> faces);