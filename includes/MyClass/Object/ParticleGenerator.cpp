#include "pch.h"
#include "ParticleGenerator.h"
#include "Collision.h"


ParticleGenerator::ParticleGenerator(Shader *shader, Camera *camera, unsigned int amount, glm::vec3 color /*= glm::vec3(0.2, 0.2, 0.7)*/)
	: shader(shader), camera(camera), amount(amount), Color(color)
{
	this->init();
}


// Generates particle when colliding to walls. No respawn.
void ParticleGenerator::Update(float dt)
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
		if (p.Color.a < 0) p.Life = 0;
	}
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
		if (p.Color.a < 0) p.Life = 0;
	}

	float omegaY = cy.GetOmega().y, v_c = vecMod(cy.GetVelocity()), v_w = abs(omegaY * cy.GetRadius());
	threashold = 0.02;

	counter += dt;
	if (counter < threashold) return;

	newParticles = int(v_c + v_w) / 20.0f;

	// Add new particles 
	for (int i = 0; i < newParticles; i++)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], cy);
	}

	counter -= std::max(dt, threashold);
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
	{
		Particle p;
		p.Color = glm::vec4(Color, 1.0);
		this->particles.push_back(p);

	}
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
	extern float currentFrame;

	float radius = cy.GetRadius();
	float randomRadian = (rand() % 63);		// 2pie = 6.28
	//float randomRadian = currentFrame;
	float randomColor = 0.5 + ((rand() % 100) / 100.0f);

	particle.Position = cy.GetPosition() + glm::vec3(radius * cos(randomRadian), 0, radius * sin(randomRadian));

	particle.Color = glm::vec4(randomColor * Color, 1.0f);

	particle.Life = 1.0f;

	float omegaY = cy.GetOmega().y;
	particle.Velocity = cy.GetVelocity() + glm::vec3(-omegaY * sin(randomRadian), 0, omegaY * cos(randomRadian));
	particle.Velocity *= (0.3 + rand() % 70 / 100);
}

// Spawn particles using collide info
void ParticleGenerator::SpawnParticle(CollisionInfo cInfo, unsigned int particleNum)
{
	// Use relative speed to determine number of particles
	// Use yz star speed as the velocity of particles
	glm::vec3 v1 = cInfo.relativeSpeed, v2 = cInfo.yzstarSpeed;
	float v1_abs = vecMod(v1), v2_abs = vecMod(v2);
	glm::vec3 v_dir = (v2_abs > 0.0001f) ? glm::normalize(v2) : ((v1_abs > 0.0001f) ? glm::normalize(v1) : glm::vec3(0));
	int num = std::min(int(0.5 * v1_abs), 30);
	
	for (int i = 0; i < num; i++)
	{

		Particle &p = particles[i];
		p.Life = 1.0f;

		p.Position = cInfo.collidePos;
		
		//p.Velocity = (i % 2) ? v2 : -v2;
		p.Velocity = -(1 + v1_abs / 20.0f) * v2;
		p.Velocity *= ((rand() % 99 / 100.0f) + 0.02);		// 0.02 ~ 1.0

		p.Color = glm::vec4(0.8f * Color, 1.0f);

	}
}
