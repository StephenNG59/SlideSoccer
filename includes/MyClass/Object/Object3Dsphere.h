#pragma once

#include "object3D.h"


class Object3Dsphere : public Object3D
{

// -- Constructor --

	public:

		Object3Dsphere(float radius = 1.0f, int widthSegments = 8, int heightSegments = 6) : radius(radius), widthSegments(widthSegments), heightSegments(heightSegments)
		{
			type = ObjectShapeType::Sphere;
			calcVertices();
			calcModelMatrix();
			//calcPhysics();
			calcInertiaMoment();
		}

		~Object3Dsphere() {}


		// -- Parameters --

	private:

		float radius;
		int widthSegments, heightSegments;


// -- Get info --

	public:

		Object3Dsphere * GetThis() { return this; }
		float GetMass() { return Object3D::GetMass(); }
		float GetRadius() { return radius; }
		glm::vec3 GetVelocity() { return Object3D::GetVelocity(); }
		float GetERestitution() { return Object3D::GetERestitution(); }


// -- Helpers --

	private:

		int index = 0;


// -- Vertices calculation --

	protected:

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
		void bindTexture(Shader shader);
	

// -- Position --

	public:

		glm::vec3 GetPosition();
		void SetPosition(glm::vec3 position);
		void ChangePosition(glm::vec3 delta);
		void UpdatePhysics(float deltaTime);


// -- Physics --

	public:

		//void calcPhysics();
		void calcOmega();
		void calcAngularMomentum();
		void calcInertiaMoment();
		void SetVelocity(glm::vec3 velocity) { Object3D::SetVelocity(velocity); }


// -- test func --
	
	public:
		
		void PrintVertices()
		{
			for (int i = 0; i < verticePositions.size(); i++)
			{
				printf("#%d# : %1f ",i , this->verticePositions[i]);
			}
		}
};