#include "stdafx.h"
#include "object3Dcube.h"


// -- Constructors --


// -- Calculate vertices --

void Object3Dcube::calcVertices()
{		
	float halfW = this->width / 2.0f;
	float halfH = this->height / 2.0f;
	float halfD = this->depth / 2.0f;

	verticeNum = 36;

	// positions
	float verticePos[36 * 3] = {		// 216
		// positions			  // normals
		-halfW, -halfH, -halfD,   
		 halfW, -halfH, -halfD,	  
		 halfW,  halfH, -halfD,	  
		 halfW,  halfH, -halfD,	  
		-halfW,  halfH, -halfD,	  
		-halfW, -halfH, -halfD,	  
								  
		-halfW, -halfH,  halfD,	  
		 halfW, -halfH,  halfD,	  
		 halfW,  halfH,  halfD,	  
		 halfW,  halfH,  halfD,	  
		-halfW,  halfH,  halfD,	  
		-halfW, -halfH,  halfD,	  
								  
		-halfW,  halfH,  halfD,	  
		-halfW,  halfH, -halfD,	  
		-halfW, -halfH, -halfD,	  
		-halfW, -halfH, -halfD,	  
		-halfW, -halfH,  halfD,	  
		-halfW,  halfH,  halfD,	  
								  
		 halfW,  halfH,  halfD,	  
		 halfW,  halfH, -halfD,	  
		 halfW, -halfH, -halfD,	  
		 halfW, -halfH, -halfD,	  
		 halfW, -halfH,  halfD,	  
		 halfW,  halfH,  halfD,	  
								  
		-halfW, -halfH, -halfD,	  
		 halfW, -halfH, -halfD,	  
		 halfW, -halfH,  halfD,	  
		 halfW, -halfH,  halfD,	  
		-halfW, -halfH,  halfD,	  
		-halfW, -halfH, -halfD,	  
								  
		-halfW,  halfH, -halfD,	  
		 halfW,  halfH, -halfD,	  
		 halfW,  halfH,  halfD,	  
		 halfW,  halfH,  halfD,	  
		-halfW,  halfH,  halfD,	  
		-halfW,  halfH, -halfD,	  
	};
	verticePositions.assign(&verticePos[0], &verticePos[108]);

	// normals
	float verticeNorm[36 * 3] = {
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,
		0.0f,  0.0f, -1.0f,

		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,

		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,

		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,
		1.0f,  0.0f,  0.0f,

		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,

		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
	};
	verticeNormals.assign(&verticeNorm[0], &verticeNorm[108]);

	float verticeUV[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f
	};
	verticeUVs.assign(&verticeUV[0], &verticeUV[72]);

	// indices
	for (int i = 0; i < verticeNum; i++)
	{
		indices.push_back(i);
	}

}


// -- Calculate model matrix --

void Object3Dcube::calcModelMatrix()
{
	Object3D::calcModelMatrix();
}


// -- Render function --

void Object3Dcube::Draw(Camera camera, Shader shader) { return Object3D::Draw(camera, shader); }


// -- Textures --

unsigned int Object3Dcube::loadTexture(char const * path) { return Object3D::loadTexture(path); }
void Object3Dcube::AddTexture(unsigned int texture, ObjectTextureType type) { Object3D::AddTexture(texture, type); }
void Object3Dcube::AddTexture(const char *texturePath, ObjectTextureType type) { Object3D::AddTexture(texturePath, type); }
void Object3Dcube::bindTexture(Shader shader) { Object3D::bindTexture(shader); }


// -- Position --

glm::vec3 Object3Dcube::GetPosition() { return Object3D::GetPosition(); }
void Object3Dcube::SetPosition(glm::vec3 position) { Object3D::SetPosition(position); }
void Object3Dcube::ChangePosition(glm::vec3 delta) { Object3D::ChangePosition(delta); }
void Object3Dcube::UpdatePhysics(float deltaTime) { Object3D::UpdatePhysics(deltaTime); }


// -- Physics --

void Object3Dcube::calcOmega()
{
	glm::vec3 L = this->GetAngularMomentum();
	if (L == glm::vec3(0.0f))
	{
		return;
	}

	calcInertiaMoment();
	
	/*glm::vec3 wDir = glm::normalize(L);
	glm::mat4 modelMatrix = this->GetModelMatrix();
	glm::vec3 x_WCnorm(1.0f, 0.0f, 0.0f), y_WCnorm(0.0f, 1.0f, 0.0f), z_WCnorm(0.0f, 0.0f, 1.0f);
	glm::vec3 x_LCnorm = glm::normalize(glm::vec3(modelMatrix * glm::vec4(x_WCnorm, 1.0f)));
	glm::vec3 y_LCnorm = glm::normalize(glm::vec3(modelMatrix * glm::vec4(y_WCnorm, 1.0f)));
	glm::vec3 z_LCnorm = glm::normalize(glm::vec3(modelMatrix * glm::vec4(z_WCnorm, 1.0f)));

	float sinA = vecMod(glm::cross(wDir, x_LCnorm)), sinB = vecMod(glm::cross(wDir, y_LCnorm)), sinC = vecMod(glm::cross(wDir, z_LCnorm));

	inertiaMoment = (1.0f / 12.0f) * mass * (width * width * sinA * sinA + height * height * sinB * sinB + depth * depth * sinC * sinC);*/

	omega = L / inertiaMoment;

}

void Object3Dcube::calcAngularMomentum()
{
	angularMomentum = omega * inertiaMoment;
}

void Object3Dcube::calcInertiaMoment()
{
	if (omega == glm::vec3(0) || mass == 0)
	{
		inertiaMoment = 0;
		return;
	}

	glm::vec3 wDir = glm::normalize(omega);

	glm::mat4 modelMatrix = this->GetModelMatrix();
	glm::vec3 x_WCnorm(1.0f, 0.0f, 0.0f), y_WCnorm(0.0f, 1.0f, 0.0f), z_WCnorm(0.0f, 0.0f, 1.0f);
	glm::vec3 x_LCnorm = glm::normalize(glm::vec3(modelMatrix * glm::vec4(x_WCnorm, 1.0f)));
	glm::vec3 y_LCnorm = glm::normalize(glm::vec3(modelMatrix * glm::vec4(y_WCnorm, 1.0f)));
	glm::vec3 z_LCnorm = glm::normalize(glm::vec3(modelMatrix * glm::vec4(z_WCnorm, 1.0f)));

	float sinA = vecMod(glm::cross(wDir, x_LCnorm)), sinB = vecMod(glm::cross(wDir, y_LCnorm)), sinC = vecMod(glm::cross(wDir, z_LCnorm));

	inertiaMoment = (1.0f / 12.0f) * mass * (width * width * sinA * sinA + height * height * sinB * sinB + depth * depth * sinC * sinC);
}

//glm::vec3 Object3Dcube::calcAirResistAcc()
//{
//	return (-velocity * air_resistance_factor);
//}
