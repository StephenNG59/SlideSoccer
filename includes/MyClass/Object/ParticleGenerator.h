#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include <MyClass/Shader/shader.h>
#include <MyClass/Object/Object3D.h>
#include <MyClass/Object/Object3Dcylinder.h>
#include <MyClass/Object/Collision.h>


struct Particle {
	glm::vec3 Position, Velocity;
	glm::vec4 Color;
	float Life;

	float CameraDistance = -1.0f;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}

	bool operator<(Particle& that)
	{
		// Sort in reverse order: far particles drawn first
		return this->CameraDistance > that.CameraDistance;
	}
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
		
		bool IsActive = true;

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





class ParticleGeneratorInstance
{
public:
	ParticleGeneratorInstance(Shader *shader, const char *texturePath, int lineNum, int columnNum);
	~ParticleGeneratorInstance();

	void Update(float dt, glm::vec3 position, glm::vec3 velocityDir, float velocityAbs, float spread, glm::vec3 cameraPos);
	void Draw(Camera *camera);
	void SetGravity(glm::vec3 g);

	bool IsActive = false;

private:
	// Basic
	int lineNum, columnNum;
	unsigned int particleCounts = 0;
	unsigned int lastUsedParticle = 0;
	Particle particleContainer[PARTICLE_MAX_AMOUNT];
	glm::vec3 gravity = PARTICLE_GRAVITY;
	// Shader
	Shader *shader;
	// VAO
	unsigned int VAO;
	// Vertex
	unsigned int VBO_billboard;
	const float g_vertex_buffer_data[30] = {
		// position			// uv
		-0.5f,  0.5f, 0,	0, 1,
		 0.5f, -0.5f, 0,	1, 0,
		 0.5f,  0.5f, 0,	1, 1,

		 0.5f, -0.5f, 0,	1, 0,
		-0.5f,  0.5f, 0,	0, 1,
		-0.5f, -0.5f, 0,	0, 0,
	};

	// Positions & life
	unsigned int VBO_pos_life;
	float g_particle_pos_life_data[PARTICLE_MAX_AMOUNT * 4];
	// Color
	unsigned int VBO_color;
	float g_particle_color_data[PARTICLE_MAX_AMOUNT * 4];
	// Texture
	unsigned int texture;

	void init();
	unsigned int firstUnusedParticle();
	void sortParticles();

	void respawnParticle(Particle &p, glm::vec3 position, glm::vec3 velocity, float velocityAbs, float spread);
};

unsigned int loadTexture(char const * path/*, bool flip_y = true*/);