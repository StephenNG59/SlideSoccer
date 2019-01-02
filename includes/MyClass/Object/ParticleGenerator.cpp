#include "pch.h"
#include "ParticleGenerator.h"
#include "Collision.h"

ParticleGenerator::ParticleGenerator(Shader * shader, Camera * camera, unsigned int amount)
	: shader(shader), camera(camera), amount(amount)
{
	this->init();
}


void ParticleGenerator::Update(float dt, Object3Dcylinder &cy, unsigned int newParticles)
{
	// Update all particles
	for (int i = 0; i < this->amount; i++)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 2.5;
		}
	}

	float omegaY = cy.GetOmega().y;
	if (omegaY == 0) threashold = 1000;
	else
	{
		threashold = 0.1 / omegaY;
	}
	counter += dt;
	if (counter < threashold) return;

	// Add new particles 
	for (int i = 0; i < newParticles; i++)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], cy);
	}

	counter -= threashold;
}

// Render all particles
void ParticleGenerator::Draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader->use();
	this->shader->setMat4("projection", camera->GetProjectionMatrix());
	this->shader->setMat4("view", camera->GetViewMatrix());
	for (Particle particle : this->particles)
	{
		if (particle.Life > 0.0f)
		{
			this->shader->setVec3("position", particle.Position);
			this->shader->setVec4("color", particle.Color);
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 12);
			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLuint VBO;
	GLfloat particle_quad[] = {
		 0.0f, -0.25f, -0.886f,
		-0.5f, -0.25f,  0.289f,
		 0.5f, -0.25f,  0.289f,
		 
		 0.0f, -0.25f, -0.886f,
		-0.5f, -0.25f,  0.289f,
		 0.0f,  0.75f,  0.000f,

		-0.5f, -0.25f,  0.289f,
		 0.5f, -0.25f,  0.289f,
		 0.0f,  0.75f,  0.000f,

		 0.5f, -0.25f,  0.289f,
		 0.0f, -0.25f, -0.886f,
		 0.0f,  0.75f,  0.000f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	/*glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));*/
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (int i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
unsigned int lastUsedParticle = 0;
unsigned int ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (int i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (int i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, Object3Dcylinder &cy)
{
	float radius = cy.GetRadius();
	float randomRadian = (rand() % 314 / 50.0f);
	float randomColor = 0.5 + ((rand() % 100) / 100.0f);

	particle.Position = cy.GetPosition() + glm::vec3(radius * cos(randomRadian), 0, radius * sin(randomRadian));

	particle.Color = glm::vec4(randomColor, randomColor, randomColor, 1.0f);

	particle.Life = 1.0f;

	float omegaY = cy.GetOmega().y;
	particle.Velocity = cy.GetVelocity() + glm::vec3(-omegaY * sin(randomRadian), 0, omegaY * cos(randomRadian));
}