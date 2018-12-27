#pragma once

#include "object3D.h"


class Object3Dcylinder : public Object3D
{

// -- Constructor --

	public:
		Object3Dcylinder(float radiusTop = 1.0f, float radiusBottom = 1.0f, float height = 1.0f, int radialSegments = 8, int heightSegments = 1) : radiusTop(radiusTop), radiusBottom(radiusBottom), height(height), radialSegments(radialSegments), heightSegments(heightSegments)
		{
			type = ObjectShapeType::Cylinder;
			// #NOTE# - verticeNum is IMPORTANT because object3D::Draw() will use it
			calcVertices();
			calcModelMatrix();
		}

		~Object3Dcylinder() {}


// -- Parameters --

	private:

		float radiusTop, radiusBottom, height;
		int radialSegments, heightSegments;

	public:

		float GetRadius();


// -- Helpers --

	private:

		int index = 0;
		float halfH = height / 2;


// -- Vertices calculation --

	protected:
		// generate torso
		void generateTorso();

		// generate vertices, normals and indices for top/bottom cap (sign = +-1)
		void generateCap(int sign);

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


// -- Physics --

	protected:

		void calcPhysics()
		{
			// inertiaMoment = 
		}

		void calcInertiaMoment();

		void calcAngularMomentum();

		void calcOmega();

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