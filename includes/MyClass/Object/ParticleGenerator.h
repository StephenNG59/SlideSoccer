#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <MyClass/Shader/shader.h>
#include <MyClass/Object/Object3D.h>
#include <MyClass/Object/Object3Dcylinder.h>


struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	float Life;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class ParticleGenerator
{

	public:

		// Constructor
		ParticleGenerator(Shader *shader, Camera *camera, unsigned int amount);

		// TODO: 添加camera到粒子系统中
		// Update all particles
		void Update(float dt, Object3Dcylinder &cy, unsigned int newParticles);

		// Render all particles
		void Draw();


	private:

		// counter
		float counter = 0, threashold = 0.05f;

		// State
		std::vector<Particle> particles;
		unsigned int amount;

		// Render state
		Shader *shader;
		unsigned int VAO;
		Camera *camera;

		// Initializes buffer and vertex attributes
		void init();

		// Returns the first Particle index that's currently unused e.g. Life <= 0
		unsigned int firstUnusedParticle();

		// Respawns particle
		void respawnParticle(Particle &particle, Object3Dcylinder &cy);

};
