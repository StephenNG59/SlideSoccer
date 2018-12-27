#include "stdafx.h"
#include "object3D.h"
#include "object3Dcube.h"
#include "object3Dcylinder.h"
#include "object3Dsphere.h"
#include "Collision.h"


float vecMod(glm::vec3 v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float vecMod(glm::vec2 v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}


// -- Get info --

Object3D * Object3D::GetThis()
{
	return this;
}

ObjectShapeType Object3D::GetType()
{
	return type;
}


// -- Model matrix --

// #TODO# - add scale, rotation and so on...
void Object3D::calcModelMatrix()
{
	modelMatrix = glm::mat4();
	//modelMatrix = rotationMatrix;
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::translate(modelMatrix, position);
	//modelMatrix = glm::rotate(modelMatrix, ......);
	modelMatrix = glm::scale(modelMatrix, scale);
	modelMatrix *= rotationMatrix;
}

glm::mat4 Object3D::GetModelMatrix() { return modelMatrix; }


glm::mat4 Object3D::GetRotationMatrix()
{
	return rotationMatrix;
}

// -- Position --

glm::vec3 Object3D::GetPosition() { return position; }


// -- Physics --

void Object3D::calcAcceleration()
{
	if (mass == 0)
	{
		acceleration = glm::vec3(0);
	}
	//else if (force != glm::vec3(0))
	//{
	//	acceleration = force / mass;
	//}
	else
	{
		acceleration = gravity;
	}
}

void Object3D::calcOmega()
{
}

void Object3D::calcAngularMomentum()
{
}

float Object3D::GetMass()
{
	return mass;
}

void Object3D::SetMass(float m)
{
	if (m == 0)
	{
		//velocity = glm::vec3(0.0f);
	}
	mass = m;
	calcInertiaMoment();
	//calcPhysics();
}

float Object3D::GetERestitution() { return eRestitution; }

void Object3D::SetERestitution(float e)
{
	eRestitution = e;
}

float Object3D::GetFriction()
{
	return friction;
}

void Object3D::SetFriction(float f)
{
	friction = f;
}

void Object3D::SetPosition(glm::vec3 position)
{
	glm::vec3 deltaPos = position - this->position;
	// update model matrix (update based on delta)
	modelMatrix = glm::translate(modelMatrix, deltaPos);
	// update position (update based on absolute, may exists some deviation from model matrix)
	this->position = position;
	//printVec3("setting position:", this->position);
	calcModelMatrix();
}
void Object3D::ChangePosition(glm::vec3 delta)
{
	// update model matrix
	modelMatrix = glm::translate(modelMatrix, delta);
	// update position
	position += delta;
	calcModelMatrix();
}
void Object3D::UpdatePhysics(float deltaTime)
{
	//// linear velocity
	//if (IsTouchingDesk)
	//{
	//	acceleration = glm::vec3(0);
	//}
	/*else
	{*/
		calcAcceleration();
	//}
		//printVec3(acceleration);
	glm::vec3 airResistAcc = calcAirResistAcc();
	//printVec3("airresist", airResistAcc);
	velocity += (acceleration + airResistAcc) * deltaTime;
	//printVec3("positionbefore", position);
	position += velocity * deltaTime;
	//printVec3("velocit", velocity);
	//printVec3("positionafter", position);
	//omega = velocity;

	//// angular velocity
	glm::vec3 airResistAngularAcc(0);
	float omegaAbsValue = vecMod(omega);
	if (omegaAbsValue != 0)
	{
		if (mass != 0)
		{
			airResistAngularAcc = calcAirResistAngularAcc();
		}
		AddOmega(airResistAngularAcc * deltaTime);
	//	omega += airResistAngularAcc * deltaTime;
		rotationMatrix = glm::rotate(rotationMatrix, omegaAbsValue * deltaTime, omega);
	}

	calcModelMatrix();
	//// update model matrix
	//modelMatrix = glm::translate(modelMatrix, deltaPos);
	//modelMatrix = rotationMatrix;
}


// -- Textures --

void Object3D::SetAmbient(glm::vec3 ambient) { this->ambient = ambient; }

void Object3D::SetDiffuse(glm::vec3 diffuse) { this->diffuse = diffuse; }

void Object3D::SetSpecular(glm::vec3 specular) { this->specular = specular; }

void Object3D::SetEmission(glm::vec3 emission) { this->emission = emission; }

void Object3D::SetShininess(float shininess) { this->shininess = shininess; }

// utility function for loading a 2D texture from file
unsigned int Object3D::loadTexture(char const * path/*, bool flip_y = true*/)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	stbi_set_flip_vertically_on_load(true);

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
int Object3D::AddTexture(unsigned int texture, ObjectTextureType type)
{
	if (textures[type].size() >= 3)		// too much texture for this type
		return -1;

	Texture newTex = {
		textureNum,		// id
		type,			// type
		texture,		// texture
		true,			// isActive
	};
	textures[type].push_back(newTex);

	textureTypeNum[type]++;
	textureNum++;

	isTextureBound = false;

	return textureNum - 1;
}
int Object3D::AddTexture(const char *texturePath, ObjectTextureType type)
{
	if (textures[type].size() >= 3)		// too much texture for this type
		return -1;

	unsigned int texture = loadTexture(texturePath);
	Texture newTex = {
		textureNum,		// id
		type,			// type
		texture,		// texture
		true,			// isActive
	};
	textures[type].push_back(newTex);

	textureTypeNum[type]++;
	textureNum++;

	isTextureBound = false;

	return textureNum - 1;
}
void Object3D::bindTexture(Shader shader)
{
	int texIndex = 0;

	for (int i = 0; i < texKindNum; i++)
	{
		//printf("textures[%d].size() = %d\n", i, textures[i].size());
		for (int j = 0; j < textures[i].size(); j++)
		{
			if (textures[i][j].isActive)
			{
				shader.setInt("material." + ObjectTextureTypeToString[i] + "Tex[" + std::to_string(j) + "]", texIndex);	// e.g. 'materialEmission2'
				glActiveTexture(GL_TEXTURE0 + texIndex);
				glBindTexture(GL_TEXTURE_2D, textures[i][j].texture);
			}
			texIndex++;
		}
	}
}


// -- Render function --

void Object3D::Draw(Camera camera, Shader shader)
{
	if (!isVAOcalculated)
	{
		calculateVAO();
		isVAOcalculated = true;
	}

	bindTexture(shader);

	modelMatrix = GetModelMatrix();

	shader.use();
	shader.setMat4("projection", camera.GetProjectionMatrix());
	shader.setMat4("view", camera.GetViewMatrix());
	shader.setMat4("model", modelMatrix);

	shader.setVec3("material.ambient", ambient);
	shader.setVec3("material.diffuse", diffuse);
	shader.setVec3("material.specular", specular);
	shader.setVec3("material.emission", emission);
	shader.setFloat("material.shininess", shininess);

	shader.setInt("ambientTexNum", textures[ObjectTextureType::Ambient].size());
	shader.setInt("diffuseTexNum", textures[ObjectTextureType::Diffuse].size());
	shader.setInt("specularTexNum", textures[ObjectTextureType::Specular].size());
	shader.setInt("emissionTexNum", textures[ObjectTextureType::Emission].size());

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, verticeNum, GL_UNSIGNED_INT, 0);
	// glDrawArrays(GL_TRIANGLES, 0, verticeNum);
}


// #NOTE air friction in the air. when on the desk, static friction will add another power on object.
glm::vec3 Object3D::calcAirResistAcc()
{
	if (mass != 0)
		if (!IsTouchingDesk)
			return (-velocity * air_resistance_factor / mass);
		else
			return (-velocity * air_resistance_factor / mass * 5.0f);
	else return glm::vec3(0);
}

// #NOTE now the angular air friction in the air is the same as on the desk, which will look weird!
glm::vec3 Object3D::calcAirResistAngularAcc()
{
	if (mass != 0)
		if (!IsTouchingDesk)
			return (-omega * air_resistance_factor / inertiaMoment / 2.0f);
		else
			return (-omega * air_resistance_factor / inertiaMoment / 2.0f * 5.0f);
	else return glm::vec3(0);
}

void Object3D::calcInertiaMoment()
{
}

//void Object3D::calcVertices()
//{
//
//}
//
//void Object3D::calcPhysics()
//{
//
//}


// -- VAO building --

// #TODO# - change the name('calculate' is not correct)
void Object3D::calculateVAO()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// #NOTE# - VAO binding should be before EBO!!
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// #NOTE# - once got wrong with the size and stuck for almost two hours
	// #NOTE# - vector's size() is the 'length' of it; pay attention to the data() form
	glBufferData(GL_ARRAY_BUFFER, (verticePositions.size() + verticeNormals.size() + verticeUVs.size()) * sizeof(float), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticePositions.size() * sizeof(float), verticePositions.data());
	glBufferSubData(GL_ARRAY_BUFFER, verticePositions.size() * sizeof(float), verticeNormals.size() * sizeof(float), verticeNormals.data());
	glBufferSubData(GL_ARRAY_BUFFER, (verticePositions.size() + verticeNormals.size()) * sizeof(float), verticeUVs.size() * sizeof(float), verticeUVs.data());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	//glBindVertexArray(VAO);
	// #TODO# - use a uniform in the shader to state the layout position of [positions] and [normals]
	// #TODO# - use some member(like [vaopIndex++]) to represent the layout position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(verticePositions.size() * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)((verticePositions.size() + verticeNormals.size()) * sizeof(float)));
	glEnableVertexAttribArray(2);
}


// -- Physics --

void Object3D::SetStatic()
{
	SetVelocity(glm::vec3(0));
	SetOmega(glm::vec3(0));
}

glm::vec3 Object3D::GetVelocity() { return velocity; }
void Object3D::SetVelocity(glm::vec3 velocity)
{
	this->velocity = velocity;
	if (this->mass > 0)
	{
	}
}
void Object3D::AddVelocity(glm::vec3 deltaVel) { this->velocity += deltaVel; }

glm::vec3 Object3D::GetOmega() { return omega; }
void Object3D::SetOmega(glm::vec3 omega) { this->omega = omega; calcInertiaMoment(); calcAngularMomentum();/*calcPhysics();*/ }

void Object3D::AddOmega(glm::vec3 delta)
{
	omega += delta;
	calcAngularMomentum();
}

glm::vec3 Object3D::GetAngularMomentum()
{
	return angularMomentum;
}

void Object3D::SetAngularMomentum(glm::vec3 am)
{
	angularMomentum = am;
	calcOmega();
}

void Object3D::AddAngularMomentum(glm::vec3 delta_am)
{
	if (mass != 0)
	{
		angularMomentum += delta_am;
		calcOmega();
	}
}

glm::vec3 Object3D::GetForce()
{
	return force;
}

void Object3D::SetForce(glm::vec3 force)
{
	this->force = force;
	//calcAcceleration();
}

void Object3D::AddForce(glm::vec3 delta_f)
{
	force += delta_f;
	//calcAcceleration();
}

glm::vec3 Object3D::GetGravity()
{
	return gravity;
}

void Object3D::SetGravity(glm::vec3 g)
{
	gravity = g;
}

void Object3D::AddGravity(glm::vec3 delta_g)
{
	gravity += delta_g;
}

float Object3D::GetAirResistanceFactor()
{
	return air_resistance_factor;
}

void Object3D::SetAirResistanceFactor(float aff)
{
	air_resistance_factor = abs(aff);
}

