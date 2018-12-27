#include "stdafx.h"
#include "object3Dsphere.h"


//void Object3Dsphere::calcPhysics()
//{
//	inertiaMoment = 0.4 * mass * radius * radius;
//	angularMomentum = inertiaMoment * omega;
//}


void Object3Dsphere::calcOmega()
{
	glm::vec3 L = angularMomentum;
	printf("calculating omega using %.2f, %.2f, %.2f\n", angularMomentum.x, angularMomentum.y, angularMomentum.z);

	omega = L / inertiaMoment;
}

void Object3Dsphere::calcAngularMomentum()
{
	angularMomentum = omega * inertiaMoment;
	//printf("calculating angular momentum, result is %.2f, %.2f, %.2f\n", angularMomentum.x, angularMomentum.y, angularMomentum.z);
}

void Object3Dsphere::calcInertiaMoment()
{
	inertiaMoment = 0.4 * mass * radius * radius;
	angularMomentum = inertiaMoment * omega;
}

//void Object3Dsphere::SetMass(float mass)
//{
//	Object3D::SetMass(mass);
//}

// -- Calculate vertices --

void Object3Dsphere::calcVertices()
{

	// vertices, normals and uvs

	for (int iy = 0; iy <= heightSegments; iy++)
	{

		float v = (float)iy / (float)heightSegments;

		for (int ix = 0; ix <= widthSegments; ix++)
		{
			
			float u = (float)ix / (float)widthSegments;

			// vertex

			float theta = glm::radians(u * 360.f);
			float phi = glm::radians(v * 180.f);
			float x = -radius * cos(theta) * sin(phi);
			float y = radius * cos(phi);
			float z = radius * sin(theta) * sin(phi);

			verticePositions.push_back(x);
			verticePositions.push_back(y);
			verticePositions.push_back(z);

			// normal

			glm::vec3 normal(x, y, z);
			glm::normalize(normal);
			verticeNormals.push_back(normal.x);
			verticeNormals.push_back(normal.y);
			verticeNormals.push_back(normal.z);

			//uv

			verticeUVs.push_back(u);
			verticeUVs.push_back(1 - v);

		}

	}

	// indices

	for (int iy = 0; iy < heightSegments; iy++)
	{

		for (int ix = 0; ix < widthSegments; ix++)
		{

			int a = iy       * (widthSegments + 1) + ix, b = a + 1;
			int c = (iy + 1) * (widthSegments + 1) + ix, d = c + 1;

			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);

			indices.push_back(b);
			indices.push_back(c);
			indices.push_back(d);

		}

	}

	verticeNum = indices.size();

}


// -- Calculate model matrix --

void Object3Dsphere::calcModelMatrix()
{
	Object3D::calcModelMatrix();
}


// -- Render function --

void Object3Dsphere::Draw(Camera camera, Shader shader) { return Object3D::Draw(camera, shader); }


// -- Textures --

unsigned int Object3Dsphere::loadTexture(char const * path) { return Object3D::loadTexture(path); }
void Object3Dsphere::AddTexture(unsigned int texture, ObjectTextureType type) { Object3D::AddTexture(texture, type); }
void Object3Dsphere::AddTexture(const char *texturePath, ObjectTextureType type) { Object3D::AddTexture(texturePath, type); }
void Object3Dsphere::bindTexture(Shader shader) { Object3D::bindTexture(shader); }


// -- Position --

glm::vec3 Object3Dsphere::GetPosition() { return Object3D::GetPosition(); }
void Object3Dsphere::SetPosition(glm::vec3 position) { Object3D::SetPosition(position); }
void Object3Dsphere::ChangePosition(glm::vec3 delta) { Object3D::ChangePosition(delta); }
void Object3Dsphere::UpdatePhysics(float deltaTime) { Object3D::UpdatePhysics(deltaTime); }
