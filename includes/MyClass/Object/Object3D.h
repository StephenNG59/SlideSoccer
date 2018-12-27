#pragma once

#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <MyClass/Shader/shader.h>
#include <MyClass/camera.h>


enum ObjectShapeType
{
	Sphere = 0,
	Cylinder = 1,
	Cube = 2,
};

enum ObjectTextureType
{
	Ambient = 0,
	Diffuse = 1,
	Specular = 2,
	Emission = 3
};

const std::string ObjectTextureTypeToString[] = {
	"ambient",
	"diffuse",
	"specular",
	"emission",
};

typedef struct __texture {
	unsigned int id;
	ObjectTextureType type;
	unsigned int texture;
	bool isActive = true;
} Texture;

const unsigned int texKindNum = 4;

float vecMod(glm::vec3 v);
float vecMod(glm::vec2 v);

class Object3D
{

	public:

		Object3D() {}
		~Object3D() {}


// -- Basic parameters --

	protected:

		ObjectShapeType type;
		unsigned int verticeNum;
		unsigned int textureNum = 0;
		unsigned int textureTypeNum[4] = { 0 };
		bool isVisible = true;


// -- Get info --

	public:

		Object3D * GetThis();
		ObjectShapeType GetType();


// -- Data to GPU --

	protected:

		std::vector<float> verticePositions;
		std::vector<float> verticeNormals;
		std::vector<float> verticeUVs;
		std::vector<unsigned int> indices;
		unsigned int VBO, VAO, EBO;


// -- Model matrix --

	protected:

		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		glm::mat4 rotationMatrix;
		glm::mat4 modelMatrix;

		void calcModelMatrix();

	public:

		glm::mat4 GetModelMatrix();
		glm::mat4 GetRotationMatrix();


// -- Position --

	public:

		glm::vec3 GetPosition();
		void SetPosition(glm::vec3 position);
		void ChangePosition(glm::vec3 delta);
		void UpdatePhysics(float deltaTime);


// -- Textures --

	protected:

		glm::vec3 ambient = glm::vec3(1.0f);
		glm::vec3 diffuse = glm::vec3(1.0f);
		glm::vec3 specular = glm::vec3(1.0f);
		glm::vec3 emission = glm::vec3(1.0f);
		float shininess = 32.0f;

		std::vector<Texture> texAmbient;
		std::vector<Texture> texDiffuse;
		std::vector<Texture> texSpecular;
		std::vector<Texture> texEmission;
		std::vector<Texture> textures[texKindNum] = {
			texAmbient,
			texDiffuse,
			texSpecular,
			texEmission
		};

	public:

		void SetAmbient(glm::vec3 ambient);
		void SetDiffuse(glm::vec3 diffuse);
		void SetSpecular(glm::vec3 specular);
		void SetEmission(glm::vec3 emission);
		void SetShininess(float shininess);

		unsigned int loadTexture(char const * path);
		int AddTexture(unsigned int texture, ObjectTextureType type);
		int AddTexture(const char *texturePath, ObjectTextureType type);

	protected:

		void bindTexture(Shader shader);


// -- Physics --

	protected:

		float mass = 1.0f;
		float friction = 0.2f;
		float eRestitution = 1.0f;
		float inertiaMoment;

		glm::vec3 velocity;
		glm::vec3 omega = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 angularMomentum = glm::vec3(0.0f);

		glm::vec3 force = glm::vec3(0);
		glm::vec3 gravity = glm::vec3(0, -9.8f, 0);
		glm::vec3 acceleration = glm::vec3(0);

		float air_resistance_factor = 0;

		void calcAcceleration();
		virtual void calcInertiaMoment();

		//virtual void calcPhysics() = 0;
		virtual void calcOmega();
		virtual void calcAngularMomentum();

	public:

		void SetStatic();

		bool IsTouchingDesk = false;
		bool IsCollidable = false;

		float GetMass();
		void SetMass(float m);

		float GetFriction();
		void SetFriction(float f);

		float GetERestitution();
		void SetERestitution(float e);

		float GetInertia() { calcInertiaMoment(); return inertiaMoment; }

		glm::vec3 GetVelocity();
		void SetVelocity(glm::vec3 velocity);
		void AddVelocity(glm::vec3 deltaVel);

		glm::vec3 GetOmega();
		void SetOmega(glm::vec3 omega);
		void AddOmega(glm::vec3 delta);

		glm::vec3 GetAngularMomentum();
		void SetAngularMomentum(glm::vec3 am);
		void AddAngularMomentum(glm::vec3 delta_am);

		glm::vec3 GetForce();
		void SetForce(glm::vec3 force);
		void AddForce(glm::vec3 delta_f);

		glm::vec3 GetGravity();
		void SetGravity(glm::vec3 g);
		void AddGravity(glm::vec3 delta_g);

		float GetAirResistanceFactor();
		void SetAirResistanceFactor(float aff);
		virtual glm::vec3 calcAirResistAcc();


// -- Helpers --

	protected:

		bool isVAOcalculated = false;
		bool isTextureBound = false;


// -- VAO building --

	protected:

		void calculateVAO();


// -- Render function --

	public:

		void Draw(Camera camera, Shader shader);


// -- Vertices data calculating --

	protected:
		
		virtual void calcVertices() = 0;

private:
	glm::vec3 calcAirResistAngularAcc();
};