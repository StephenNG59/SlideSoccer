#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <MyClass/Shader/shader.h>
#include <MyClass/Object/Object3D.h>
#include <MyClass/Object/Object3Dcylinder.h>
#include <MyClass/Object/Collision.h>


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
		ParticleGenerator(Shader *shader, Camera *camera, unsigned int amount, glm::vec3 color = glm::vec3(0.2, 0.3, 0.4));

		// Update all particles
		void Update(float dt);
		void Update(float dt, Object3Dcylinder &cy, unsigned int newParticles);

		// Render all particles
		void Draw();

		glm::vec3 Color;


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

public:
	void SpawnParticle(CollisionInfo cInfo, unsigned int particleNum);
};
