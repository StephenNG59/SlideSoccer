#pragma once

#include "pch.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>

class Camera
{
public:
	// Constructor with vectors
	Camera(glm::vec3 eyeCor = glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3 centerCor = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f)) : EyeCor(eyeCor), CenterCor(centerCor), UpVecNorm(upVec)
	{
		updateCameraVectors();
	}

	// Constructor with scaler values
	Camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
	{
		EyeCor = glm::vec3(eyeX, eyeY, eyeZ);
		CenterCor = glm::vec3(centerX, centerY, centerZ);
		UpVecNorm = glm::vec3(upX, upY, upZ);
		updateCameraVectors();
	}

	// Coordinates
	glm::vec3 EyeCor;
	glm::vec3 CenterCor;

	// Normal vectors
	glm::vec3 UpVecNorm;
	glm::vec3 lookVecNorm;
	glm::vec3 rightVecNorm;
	// Non-normal vectors
	glm::vec3 lookVec;

	void SmoothlyMoveTo(glm::vec3 destPos, glm::vec3 destCenter, glm::vec3 destUpVec, float totalTime)
	{
		isMoving = true;
		movingTotalTime = totalTime;

		this->destPos = destPos;
		this->destCenter = destCenter;
		this->destUpVecNorm = glm::normalize(destUpVec);	// TODO: not safe!!
	}

	void Update(float dt)
	{
		if (!isMoving) return;

		float delta = std::min(abs(dt / movingTotalTime), 1.0f);
		glm::vec3 dPos = destPos - EyeCor;
		glm::vec3 dCenter = destCenter - CenterCor;
		glm::vec3 dUpVecNorm = destUpVecNorm - UpVecNorm;
		if (vecMod(dPos) < 0.05 && vecMod(dCenter) < 0.05)
		{
			EyeCor = destPos;
			CenterCor = destCenter;
			UpVecNorm = destUpVecNorm;
			updateCameraVectors();
			isMoving = false;
			return;
		}

		dPos *= delta;
		dCenter *= delta;
		dUpVecNorm *= delta;

		EyeCor += dPos;
		CenterCor += dCenter;
		//UpVecNorm += dUpVecNorm;
		UpVecNorm = glm::normalize(UpVecNorm + dUpVecNorm);

		updateCameraVectors();
	}

private:
	// Smothly moving
	bool isMoving = false;
	float movingTotalTime = 2;
	glm::vec3 destPos = CAMERA_POS_1;
	glm::vec3 destCenter = CAMERA_CENTER_1;
	glm::vec3 destUpVecNorm = CAMERA_UPVECNORM_1;


	// Matrix
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// Perspective
	float fov, aspect, zNear, zFar;			// #TODO make sure to initialize

	// Update vectors and matrix based on eye coord and center coord
	void updateCameraVectors()
	{
		lookVec = CenterCor - EyeCor;
		lookVecNorm = glm::normalize(lookVec);
		rightVecNorm = glm::normalize(glm::cross(lookVecNorm, UpVecNorm));
		UpVecNorm = glm::normalize(glm::cross(rightVecNorm, lookVecNorm));

		viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
	}
	void updateProjectionMatrix()
	{
		projectionMatrix = glm::perspective(fov, aspect, zNear, zFar);
	}

public:
	float Fov = 45.0f;

	void SetPerspective(float fov, float aspect, float zNear, float zFar)
	{
		this->fov = fov;
		this->aspect = aspect;
		this->zNear = zNear;
		this->zFar = zFar;
		updateProjectionMatrix();
	}

	// Get the eye position
	glm::vec3 GetPosition() { return EyeCor; }

	// Get front direction
	glm::vec3 GetFrontDirection() { return lookVecNorm; }

	// Get view matrix
	glm::mat4 GetViewMatrix() { return viewMatrix; }

	// Get projection matrix
	glm::mat4 GetProjectionMatrix() { return projectionMatrix; }

	// Go up
	void GoUp(float distance)
	{
		EyeCor += distance * UpVecNorm;
		CenterCor += distance * UpVecNorm;
	}

	// Go right
	void GoRight(float distance)
	{
		EyeCor += distance * rightVecNorm;
		CenterCor += distance * rightVecNorm;
		viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
	}

	// Go forward
	void GoForward(float distance)
	{
		EyeCor += distance * lookVecNorm;
		CenterCor += distance * lookVecNorm;
		viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
	}

	// Orient camera direction w.r.t up axis
	void OrientLeftByDegree(float degree)
	{
		glm::mat4 trans;
		trans = glm::rotate(trans, glm::radians(degree), UpVecNorm);

		rightVecNorm = glm::vec3(trans * glm::vec4(rightVecNorm, 1.0f));
		lookVec = glm::vec3(trans * glm::vec4(lookVec, 1.0f));
		lookVecNorm = glm::normalize(lookVec);
		CenterCor = EyeCor + lookVec;

		//viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);

		updateCameraVectors();
	}

	// Orient camera direction w.r.t right axis
	void OrientUpByDegree(float degree)
	{
		glm::mat4 trans;
		trans = glm::rotate(trans, glm::radians(degree), rightVecNorm);

		UpVecNorm = glm::vec3(trans * glm::vec4(UpVecNorm, 1.0f));
		lookVec = glm::vec3(trans * glm::vec4(lookVec, 1.0f));
		lookVecNorm = glm::normalize(lookVec);
		CenterCor = EyeCor + lookVec;

		//viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
		updateCameraVectors();
	}

	// Rotate camera w.r.t up axis on center coord
	void RotateRightByDegree(float degree)
	{
		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(CenterCor));
		trans = glm::rotate(trans, glm::radians(degree), UpVecNorm);
		trans = glm::translate(trans, -glm::vec3(CenterCor));

		EyeCor = glm::vec3(trans * glm::vec4(EyeCor, 1.0f));

		OrientLeftByDegree(degree);
	}

	// Rotate camera w.r.t up axis on rotate point
	void RotateRightByDegree(float degree, glm::vec3 rotatePoint)
	{
		if (rotatePoint == EyeCor)
			return;

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(rotatePoint));
		trans = glm::rotate(trans, glm::radians(degree), UpVecNorm);
		trans = glm::translate(trans, -glm::vec3(rotatePoint));

		EyeCor = glm::vec3(trans * glm::vec4(EyeCor, 1.0f));

		OrientLeftByDegree(degree);
	}

	// Rotate camera w.r.t right axis on center coord
	void RotateDownByDegree(float degree)
	{
		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(CenterCor));
		trans = glm::rotate(trans, glm::radians(degree), rightVecNorm);
		trans = glm::translate(trans, -glm::vec3(CenterCor));

		EyeCor = glm::vec3(trans * glm::vec4(EyeCor, 1.0f));

		OrientUpByDegree(degree);
	}

	// Rotate camera w.r.t right axis on rotate point
	void RotateDownByDegree(float degree, glm::vec3 rotatePoint)
	{
		if (rotatePoint == EyeCor)
			return;

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(rotatePoint));
		trans = glm::rotate(trans, glm::radians(degree), rightVecNorm);
		trans = glm::translate(trans, -glm::vec3(rotatePoint));

		EyeCor = glm::vec3(trans * glm::vec4(EyeCor, 1.0f));

		OrientUpByDegree(degree);
	}

	// Rotate camera w.r.t rotate axis on center coord
	void RotateCounterClockByDegree(float degree, glm::vec3 rotateAxis)
	{
		glm::vec3 rotatePoint = CenterCor;

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(rotatePoint));
		trans = glm::rotate(trans, glm::radians(degree), rotateAxis);
		trans = glm::translate(trans, -glm::vec3(rotatePoint));

		EyeCor = glm::vec3(trans * glm::vec4(EyeCor, 1.0f));

		updateCameraVectors();
		viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
	}

	// Rotate camera w.r.t rotate axis on rotate point
	void RotateCounterClockByDegree(float degree, glm::vec3 rotateAxis, glm::vec3 rotatePoint)
	{
		if (rotatePoint == EyeCor)
			return;

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(rotatePoint));
		trans = glm::rotate(trans, glm::radians(degree), rotateAxis);
		trans = glm::translate(trans, -glm::vec3(rotatePoint));

		EyeCor = glm::vec3(trans * glm::vec4(EyeCor, 1.0f));

		updateCameraVectors();
		viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
	}

	void ShowStatus()
	{
		std::cout << "\n-----------------" << std::endl;
		std::cout << "eyeCor: " << EyeCor.x << " # " << EyeCor.y << " # " << EyeCor.z << std::endl;
		std::cout << "centerCor: " << CenterCor.x << " # " << CenterCor.y << " # " << CenterCor.z << std::endl;
		std::cout << "upVecNorm: " << UpVecNorm.x << " # " << UpVecNorm.y << " # " << UpVecNorm.z << std::endl;
		std::cout << "lookVecNorm: " << lookVecNorm.x << " # " << lookVecNorm.y << " # " << lookVecNorm.z << std::endl;
		std::cout << "rightVecNorm: " << rightVecNorm.x << " # " << rightVecNorm.y << " # " << rightVecNorm.z << std::endl;
		std::cout << "mod of lookVec: " << glm::dot(lookVec, lookVecNorm) << std::endl;
	}


};


