#include "pch.h"
#include "ParticleGenerator.h"
#include "Collision.h"


#pragma region ParticleGenerator Origin Version


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
			p.Color.a -= dt * 1.5;
			if (p.Color.a <= 0) p.Life = 0;
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
			p.Color.a -= dt * 1.5;
		}
		if (p.Color.a < 0) p.Life = 0;
	}

	float omegaY = cy.GetOmega().y, v_c = vecMod(cy.GetVelocity()), v_w = abs(omegaY * cy.GetRadius());
	threashold = 0.02;

	/*counter += dt;
	if (counter < threashold) return;*/

	newParticles = (v_c * 2 + v_w) / 10.0f + 1;
	//newParticles *= 2;

	// Add new particles 
	for (int i = 0; i < newParticles; i++)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], cy);
	}

	//counter -= std::max(dt, threashold);
}

void ParticleGenerator::Update(float dt, Object3Dsphere &cy, unsigned int newParticles)
{
	// Update all particles
	for (int i = 0; i < this->amount; i++)
	{
		Particle &p = this->particles[i];
		p.Life -= dt; // reduce life
		if (p.Life > 0.0f)
		{	// particle is alive, thus update
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 1.5;
		}
		if (p.Color.a < 0) p.Life = 0;
	}

	float omegaY = cy.GetOmega().y, v_c = vecMod(cy.GetVelocity()), v_w = abs(omegaY * cy.GetRadius());
	threashold = 0.02;

	/*counter += dt;
	if (counter < threashold) return;*/

	newParticles = (v_c * 2 + v_w) / 10.0f + 1;
	//newParticles *= 2;

	// Add new particles 
	for (int i = 0; i < newParticles; i++)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], cy);
	}

	//counter -= std::max(dt, threashold);
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
	float size = 0.3;
	/*GLfloat particle_quad[] = {
		 0.0f * size, -0.25f * size, -0.886f * size,
		-0.5f * size, -0.25f * size,  0.289f * size,
		 0.5f * size, -0.25f * size,  0.289f * size,

		 0.0f * size, -0.25f * size, -0.886f * size,
		-0.5f * size, -0.25f * size,  0.289f * size,
		 0.0f * size,  0.75f * size,  0.000f * size,

		-0.5f * size, -0.25f * size,  0.289f * size,
		 0.5f * size, -0.25f * size,  0.289f * size,
		 0.0f * size,  0.75f * size,  0.000f * size,

		 0.5f * size, -0.25f * size,  0.289f * size,
		 0.0f * size, -0.25f * size, -0.886f * size,
		 0.0f * size,  0.75f * size,  0.000f * size
	}; */
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
	//glVertexAttribDivisor(0, 0);

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

	float omegaY = cy.GetOmega().y;
	glm::vec3 velocity = cy.GetVelocity();
	//float disFromCen = std::max(0.0f, 1 - 0.3f * vecMod(velocity) / (abs(omegaY) + 0.1f));
	particle.Position = cy.GetPosition() + glm::vec3(radius * cos(randomRadian), 0, radius * sin(randomRadian)) * (rand() % 10 / 10.0f) /* * disFromCen*/;

	particle.Color = glm::vec4(randomColor * Color, 1.0f);

	particle.Life = 1.0f;

	particle.Velocity = velocity + glm::vec3(-omegaY * sin(randomRadian), /*rand() * 27 / (float)RAND_MAX - 3*/0, omegaY * cos(randomRadian));
	particle.Velocity *= (0.3 + rand() % 70 / 100);
	//particle.Velocity *= 0.8;
}

void ParticleGenerator::respawnParticle(Particle &particle, Object3Dsphere &cy)
{
	extern float currentFrame;

	float radius = cy.GetRadius();
	float randomRadian = (rand() % 63);		// 2pie = 6.28
	//float randomRadian = currentFrame;
	float randomColor = 0.5 + ((rand() % 100) / 100.0f);

	float omegaY = cy.GetOmega().y;
	glm::vec3 velocity = cy.GetVelocity();
	//float disFromCen = std::max(0.0f, 1 - 0.3f * vecMod(velocity) / (abs(omegaY) + 0.1f));
	particle.Position = cy.GetPosition() + glm::vec3(radius * cos(randomRadian), 0, radius * sin(randomRadian)) * (rand() % 10 / 10.0f) /* * disFromCen*/;

	particle.Color = glm::vec4(randomColor * Color, 1.0f);

	particle.Life = 1.0f;

	particle.Velocity = velocity + glm::vec3(-omegaY * sin(randomRadian), /*rand() * 27 / (float)RAND_MAX - 3*/0, omegaY * cos(randomRadian));
	particle.Velocity *= (0.3 + rand() % 70 / 100);
	//particle.Velocity *= 0.8;
}

// Spawn particles using collide info
void ParticleGenerator::SpawnParticle(CollisionInfo cInfo, unsigned int particleNum)
{
	// Use relative speed to determine number of particles
	// Use yz star speed as the velocity of particles
	glm::vec3 v1 = cInfo.relativeSpeed, v2 = cInfo.yzstarSpeed;
	float v1_abs = vecMod(v1), v2_abs = vecMod(v2);
	glm::vec3 v_dir = (v2_abs > 0.0001f) ? glm::normalize(v2) : ((v1_abs > 0.0001f) ? glm::normalize(v1) : glm::vec3(0));
	int num = std::min(int(1 * v1_abs), 60);

	for (int i = 0; i < num; i++)
	{

		Particle &p = particles[i];
		p.Life = 1.0f;

		p.Position = cInfo.collidePos + glm::vec3(rand() % 61 / 30.0f - 1.0f, 0, rand() % 61 / 30.0f - 1.0f);

		//p.Velocity = (i % 2) ? v2 : -v2;
		p.Velocity = -(1 + v1_abs / 20.0f) * v2;
		p.Velocity *= ((rand() % 69 / 100.0f) + 0.02);		// 0.02 ~ 0.7

		p.Color = glm::vec4(0.8f * Color, 1.0f);

	}
}


#pragma endregion




// --------------------------------------------------------------------------------------------


ParticleGeneratorInstance::ParticleGeneratorInstance(Shader *shader, const char *texturePath, int lineNum, int columnNum) : lineNum(lineNum), columnNum(columnNum)
{
	this->shader = shader;
	
	// Load texture
	texture = loadTexture(texturePath);
	
	this->init();
}


void ParticleGeneratorInstance::Update(float dt, glm::vec3 position, glm::vec3 velocityDir, float velocityAbs, float spread, glm::vec3 cameraPos)
{

	if (IsActive)
	{
		// Respawn particles
		int newParticles = (int)(dt * PARTICLE_PER_SECOND);
		if (newParticles > (int)(0.016 * PARTICLE_PER_SECOND))
			newParticles = (int)(0.016 * PARTICLE_PER_SECOND);

		for (int i = 0; i < newParticles; i++)
		{
			int particleIndex = firstUnusedParticle();
			respawnParticle(particleContainer[particleIndex], position, velocityDir, velocityAbs, spread);
		}
	}

	// Update status of particles, and update data arrays
	particleCounts = 0;
	for (int i = 0; i < PARTICLE_MAX_AMOUNT; i++)
	{
		Particle& p = particleContainer[i];

		if (p.Life > 0.0f)
		{

			p.Life -= dt;
			if (p.Life > 0.0f)
			{
				p.Velocity += gravity * dt;
				p.Position += p.Velocity * dt;
				p.CameraDistance = glm::length2(p.Position - cameraPos);

				g_particle_pos_life_data[4 * particleCounts + 0] = p.Position.x;
				g_particle_pos_life_data[4 * particleCounts + 1] = p.Position.y;
				g_particle_pos_life_data[4 * particleCounts + 2] = p.Position.z;
				
				g_particle_pos_life_data[4 * particleCounts + 3] = p.Life;

				g_particle_color_data[4 * particleCounts + 0] = p.Color.r;
				g_particle_color_data[4 * particleCounts + 1] = p.Color.g;
				g_particle_color_data[4 * particleCounts + 2] = p.Color.b;
				g_particle_color_data[4 * particleCounts + 3] = p.Color.a;
			}
			else
			{
				p.CameraDistance = -1.0f;
			}

			particleCounts++;
		}
	}

	// Sort particles
	//sortParticles();

	// Update pos and life buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos_life);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_MAX_AMOUNT * 4 * sizeof(float), NULL, GL_STREAM_DRAW);// Buffer orphaning, a common way to improve streaming performance.
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCounts * 4 * sizeof(float), g_particle_pos_life_data);

	// Update color buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_MAX_AMOUNT * 4 * sizeof(float), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, particleCounts * 4 * sizeof(float), g_particle_color_data);

}

void ParticleGeneratorInstance::Draw(Camera *camera)
{
	glDepthMask(GL_FALSE);

	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glBindVertexArray(VAO);
	this->shader->use();
	this->shader->setMat4("view", camera->GetViewMatrix());
	this->shader->setMat4("projection", camera->GetProjectionMatrix());
	this->shader->setFloat("maxLife", PARTICLE_LIFE);
	this->shader->setVec3("cameraRight", camera->GetRightDirection());
	this->shader->setVec3("cameraUp", camera->GetUpDirection());
	this->shader->setInt("lineNum", lineNum);
	this->shader->setInt("columnNum", columnNum);

	// Texture binding
	shader->setInt("myTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);


	// 1st attribute buffer: vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_billboard);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribDivisor(0, 0);		// Always reuse
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_billboard);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribDivisor(1, 0);		// Always reuse

	// 2nd attribute buffer: pos & life
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos_life);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(2, 1);		// One per quad

	// 3rd attribute buffer: color
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(3, 1);		// One per quad

	// Draw
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particleCounts);

	// Change back to default
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthMask(GL_TRUE);
}


void ParticleGeneratorInstance::SetGravity(glm::vec3 g)
{
	gravity = g;
}

void ParticleGeneratorInstance::init()
{
	// VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// The VBO containing the 6(4) vertices of the particles
	glGenBuffers(1, &VBO_billboard);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_billboard);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// The VBO containing the positions and life of the particles
	glGenBuffers(1, &VBO_pos_life);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_pos_life);
	// Initialize with empty (NULL) buffer. It will be updated each frame with GL_STREAM_DRAW.
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_MAX_AMOUNT * 4 * sizeof(float), NULL, GL_STREAM_DRAW);

	// The VBO containing the colors of the particles
	glGenBuffers(1, &VBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_color);
	// Initialize with empty (NULL) buffer. It will be updated each frame with GL_STREAM_DRAW.
	glBufferData(GL_ARRAY_BUFFER, PARTICLE_MAX_AMOUNT * 4 * sizeof(float), NULL, GL_STREAM_DRAW);

	for (int i = 0; i < PARTICLE_MAX_AMOUNT; i++)
	{
		particleContainer[i].Color = glm::vec4(PARTICLE_COLOR_BLUE, 1.0f);
	}

}

unsigned int ParticleGeneratorInstance::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (int i = lastUsedParticle; i < PARTICLE_MAX_AMOUNT; ++i) {
		if (this->particleContainer[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (int i = 0; i < lastUsedParticle; ++i) {
		if (this->particleContainer[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGeneratorInstance::sortParticles()
{
	std::sort(&particleContainer[0], &particleContainer[PARTICLE_MAX_AMOUNT]);
}

void ParticleGeneratorInstance::respawnParticle(Particle &p, glm::vec3 position, glm::vec3 velocityDir, float velocityAbs, float spread)
{
	// 
	if (velocityAbs == 0) return;
	// Explosion-like: all directions
	if (velocityDir == glm::vec3(0))
	{
		float x = rand() % 11 / 10.0f, y = rand() % 11 / 10.0f, z = rand() % 11 / 10.0f;
		glm::vec3 dir = glm::normalize(glm::vec3(x, y, z));
		int negative = rand() % 8;
		if (negative / 4) dir.x = -dir.x;
		negative %= 4;
		if (negative / 2) dir.y = -dir.y;
		if (negative % 2) dir.z = -dir.z;

		p.Velocity = dir * velocityAbs;
	}
	// Focus on specific direction
	else if (spread == 0)
	{
		p.Velocity = velocityDir * velocityAbs;
	}
	// Spread
	else
	{
		// Casually choose a vector, cross with velocity and find a vector that's perpendicular to them (emmm...)
		glm::vec3 v_prime = velocityDir + glm::vec3(1.7, 1.9, 0);
		glm::vec3 per_1 = glm::normalize(glm::cross(velocityDir, v_prime));
		glm::vec3 per_2 = glm::normalize(glm::cross(velocityDir, per_1));
		float theta = rand() % 628;
		float spread_12 = rand() % 101 / 100.0f * spread;
		p.Velocity = (glm::normalize(velocityDir) + spread_12 * (sin(theta) * per_1 + cos(theta) * per_2)) * velocityAbs * (0.95f + rand() % 11 / 100.0f);

	}


	p.Life = PARTICLE_LIFE;
	p.Color = glm::vec4(PARTICLE_COLOR_BLUE, 1.0f);
	p.Position = position/* + glm::vec3(rand() % 101 / 10.0f - 5.0f, rand() % 101 / 10.0f - 5.0f, rand() % 101 / 10.0f - 5.0f)*/;

	//p.Velocity = velocity + glm::vec3(rand() % 10 / 5.0f - 1.0f, rand() % 10 / 5.0f + 10, rand() % 10 / 5.0f - 1.0f);
}

unsigned int loadTexture(char const * path/*, bool flip_y = true*/)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//stbi_set_flip_vertically_on_load(true);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}