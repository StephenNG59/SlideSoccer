#include "stdafx.h"
#include "object3Dcylinder.h"


float Object3Dcylinder::GetRadius()
{
	return (radiusBottom + radiusTop) * 0.5f;
}

// -- Calculate vertices --

void Object3Dcylinder::generateTorso()
{
	float vx, vy, vz;
	glm::vec3 normal;

	// this will be used to calculate the normal
	float slope = (radiusBottom - radiusTop) / height;

	// generate vertices, normals and uvs for torso
	for (int y = 0; y <= heightSegments; y++)
	{
		std::vector<int> indexRow;

		float v = (float)y / (float)heightSegments;

		float radius = v * (radiusBottom - radiusTop) + radiusTop;

		for (int x = 0; x <= radialSegments; x++)
		{
			float u = (float)x / (float)radialSegments;

			float theta = glm::radians(360.0f * u);

			float sinTheta = sin(theta), cosTheta = cos(theta);

			// vertex
			vx = radius * sinTheta;
			vy = halfH - v * height; 
			vz = radius * cosTheta;
			verticePositions.push_back(vx);
			verticePositions.push_back(vy);
			verticePositions.push_back(vz);

			// normal
			vx = sinTheta;
			vy = slope;
			vz = cosTheta;
			glm::normalize(normal);
			//verticeNormals.push_back(normal.x, normal.y, normal.z);
			verticeNormals.push_back(vx);	// for test
			verticeNormals.push_back(vy);	// for test
			verticeNormals.push_back(vz);	// for test

			// uv
			verticeUVs.push_back(u);
			verticeUVs.push_back(1-v);

			// indices of torso
			if (y >= 1)
			{
				int a = (y - 1) * (radialSegments + 1) + x - 1, b = a + 1;
				int c =       y * (radialSegments + 1) + x - 1, d = c + 1;
				
				indices.push_back(a);
				indices.push_back(b);
				indices.push_back(c);
				
				indices.push_back(b);
				indices.push_back(c);
				indices.push_back(d);
			}

			index++;
		}
	}
}

void Object3Dcylinder::generateCap(int sign)
{
	float vx, vy, vz;
	int centerIndexStart = index;
	float radius = (sign == 1) ? radiusTop : radiusBottom;

	// vertices, normals and uvs for center
	for (int x = 0; x <= radialSegments; x++)
	{
		// vertex
		vx = 0.0f;
		vy = sign * halfH;
		vz = 0.0f;
		verticePositions.push_back(vx);
		verticePositions.push_back(vy);
		verticePositions.push_back(vz);

		// normal
		verticeNormals.push_back(0.0f);
		verticeNormals.push_back(sign * 1.0f);
		verticeNormals.push_back(0.0f);

		// uv
		verticeUVs.push_back(0.5f);
		verticeUVs.push_back(0.5f);

		index++;
	}

	int centerIndexEnd = index;

	// vertices, normals and uvs for surrounding points
	for (int x = 0; x <= radialSegments; x++)
	{
		float u = (float)x / (float)radialSegments;
		float theta = glm::radians(u * 360.0f);

		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		// vertex
		vx = radius * sinTheta;
		vy = halfH * sign;
		vz = radius * cosTheta;
		verticePositions.push_back(vx);
		verticePositions.push_back(vy);
		verticePositions.push_back(vz);

		// normal
		verticeNormals.push_back(0);
		verticeNormals.push_back(sign);
		verticeNormals.push_back(0);

		// uv
		verticeUVs.push_back(0.5f + cosTheta * 0.5f       );
		verticeUVs.push_back(0.5f + sinTheta * 0.5f * sign);

		index++;
	}

	for (int x = 0; x < radialSegments; x++) {

		int c = centerIndexStart + x;
		int i = centerIndexEnd + x;

		// face top
		if (sign == 1) {
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(c);
		}
		// face bottom
		else {
			indices.push_back(i + 1);
			indices.push_back(i);
			indices.push_back(c);
		}
	}
}

void Object3Dcylinder::calcVertices()
{		
	// generate torso
	generateTorso();

	// generate cap
	generateCap(1);
	generateCap(-1);

	verticeNum = indices.size();
}


// -- Calculate model matrix --

void Object3Dcylinder::calcModelMatrix()
{
	Object3D::calcModelMatrix();
}


// -- Render function --

void Object3Dcylinder::Draw(Camera camera, Shader shader) { return Object3D::Draw(camera, shader); }


// -- Textures --

unsigned int Object3Dcylinder::loadTexture(char const * path) { return Object3D::loadTexture(path); }
void Object3Dcylinder::AddTexture(unsigned int texture, ObjectTextureType type) { Object3D::AddTexture(texture, type); }
void Object3Dcylinder::AddTexture(const char *texturePath, ObjectTextureType type) { Object3D::AddTexture(texturePath, type); }
void Object3Dcylinder::bindTexture(Shader shader) { Object3D::bindTexture(shader); }


// -- Position --

glm::vec3 Object3Dcylinder::GetPosition() { return Object3D::GetPosition(); }
void Object3Dcylinder::SetPosition(glm::vec3 position) { Object3D::SetPosition(position); }
void Object3Dcylinder::ChangePosition(glm::vec3 delta) { Object3D::ChangePosition(delta); }
void Object3Dcylinder::UpdatePhysics(float deltaTime) { Object3D::UpdatePhysics(deltaTime); }

void Object3Dcylinder::calcInertiaMoment()
{
	inertiaMoment = 0.4f * mass * 0.5f * (radiusBottom + radiusTop);		// #NOTE this is not correct but who cares
}

void Object3Dcylinder::calcAngularMomentum()
{
	angularMomentum = omega * inertiaMoment;
}

void Object3Dcylinder::calcOmega()
{
	glm::vec3 L = angularMomentum;
	omega = L / inertiaMoment;
}