#pragma once

#include "object3D.h"

class Object3Dcube : public Object3D
{

// -- Constructors --

	public:

	// Constructor with vectors
	Object3Dcube(glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f)) : width(size.x), height(size.y), depth(size.z)
	{
		type = ObjectShapeType::Cube;
		// this->position = position;
		calcVertices();
		calcModelMatrix();
	}

	// Constructor with scaler values
	Object3Dcube(float width = 1.0f, float height = 1.0f, float depth = 1.0f) : width(width), height(height), depth(depth)
	{
		type = ObjectShapeType::Cube;
		// this->position = glm::vec3(posX, posY, posZ);
		calcVertices();
		calcModelMatrix();
	}

	~Object3Dcube() {}


// -- Parameters --

	private:

		float width;
		float height;
		float depth;

	public:

		float GetWidth() { return width; }
		float GetHeight() { return height; }
		float GetDepth() { return depth; }


// -- Vertices calculation --

	private:

		void calcVertices();


// -- Calculate model matrix --

	protected:

		void calcModelMatrix();


// -- Render function --

	public:

		void Draw(Camera camera, Shader shader);


// -- Textures --

	public:

		unsigned int loadTexture(char const * path);
		void AddTexture(unsigned int texture, ObjectTextureType type);
		void AddTexture(const char *texturePath, ObjectTextureType type);

	protected:
	
		void bindTexture(Shader shader);
	

// -- Position --

	public:

		glm::vec3 GetPosition();
		void SetPosition(glm::vec3 position);
		void ChangePosition(glm::vec3 delta);
		void UpdatePhysics(float deltaTime);

		void PrintVertices()
		{
			for (int i = 0; i < indices.size(); ++i)
			{
				printf("%d\n", indices[i]);
			}
		}


// -- Physics --

	protected:

		void calcOmega();
		void calcAngularMomentum();
		void calcInertiaMoment();
		//glm::vec3 calcAirResistAcc();

		//void calcPhysics()
		//{
		//	// inertiaMoment = 
		//}
};