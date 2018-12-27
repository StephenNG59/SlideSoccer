#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Camera
{
public:
	// Constructor with vectors
	Camera(glm::vec3 eyeCor = glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3 centerCor = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f)) : eyeCor(eyeCor), centerCor(centerCor), upVecNorm(upVec)
	{
		updateCameraVectors();
	}

	// Constructor with scaler values
	Camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
	{
		eyeCor = glm::vec3(eyeX, eyeY, eyeZ);
		centerCor = glm::vec3(centerX, centerY, centerZ);
		upVecNorm = glm::vec3(upX, upY, upZ);
		updateCameraVectors();
	}

private:
	// Coordinates
	glm::vec3 eyeCor;
	glm::vec3 centerCor;
	// Normal vectors
	glm::vec3 upVecNorm;
	glm::vec3 lookVecNorm;
	glm::vec3 rightVecNorm;
	// Non-normal vectors
	glm::vec3 lookVec;

	// Matrix
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// Perspective
	float fov, aspect, zNear, zFar;			// #TODO make sure to initialize

	// Update vectors and matrix based on eye coord and center coord
	void updateCameraVectors()
	{
		lookVec = centerCor - eyeCor;
		lookVecNorm = glm::normalize(lookVec);
		rightVecNorm = glm::normalize(glm::cross(lookVecNorm, upVecNorm));
		upVecNorm = glm::normalize(glm::cross(rightVecNorm, lookVecNorm));

		viewMatrix = glm::lookAt(eyeCor, centerCor, upVecNorm);
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
	glm::vec3 GetPosition() { return eyeCor; }

	// Get front direction
	glm::vec3 GetFrontDirection() { return lookVecNorm; }

	// Get view matrix
	glm::mat4 GetViewMatrix() { return viewMatrix; }

	// Get projection matrix
	glm::mat4 GetProjectionMatrix() { return projectionMatrix; }

	// Go up
	void GoUp(float distance)
	{
		eyeCor += distance * upVecNorm;
		centerCor += distance * upVecNorm;
	}

	// Go right
	void GoRight(float distance)
	{
		eyeCor += distance * rightVecNorm;
		centerCor += distance * rightVecNorm;
	}

	// Go forward
	void GoForward(float distance)
	{
		eyeCor += distance * lookVecNorm;
		centerCor += distance * lookVecNorm;
	}

	// Orient camera direction w.r.t up axis
	void OrientLeftByDegree(float degree)
	{
		glm::mat4 trans;
		trans = glm::rotate(trans, glm::radians(degree), upVecNorm);

		rightVecNorm = glm::vec3(trans * glm::vec4(rightVecNorm, 1.0f));
		lookVec = glm::vec3(trans * glm::vec4(lookVec, 1.0f));
		lookVecNorm = glm::normalize(lookVec);
		centerCor = eyeCor + lookVec;

		viewMatrix = glm::lookAt(eyeCor, centerCor, upVecNorm);
	}

	// Orient camera direction w.r.t right axis
	void OrientUpByDegree(float degree)
	{
		glm::mat4 trans;
		trans = glm::rotate(trans, glm::radians(degree), rightVecNorm);

		upVecNorm = glm::vec3(trans * glm::vec4(upVecNorm, 1.0f));
		lookVec = glm::vec3(trans * glm::vec4(lookVec, 1.0f));
		lookVecNorm = glm::normalize(lookVec);
		centerCor = eyeCor + lookVec;

		viewMatrix = glm::lookAt(eyeCor, centerCor, upVecNorm);
	}

	// Rotate camera w.r.t up axis on center coord
	void RotateRightByDegree(float degree)
	{
		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(centerCor));
		trans = glm::rotate(trans, glm::radians(degree), upVecNorm);
		trans = glm::translate(trans, -glm::vec3(centerCor));

		eyeCor = glm::vec3(trans * glm::vec4(eyeCor, 1.0f));

		OrientLeftByDegree(degree);
	}

	// Rotate camera w.r.t up axis on rotate point
	void RotateRightByDegree(float degree, glm::vec3 rotatePoint)
	{
		if (rotatePoint == eyeCor)
			return;

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(rotatePoint));
		trans = glm::rotate(trans, glm::radians(degree), upVecNorm);
		trans = glm::translate(trans, -glm::vec3(rotatePoint));

		eyeCor = glm::vec3(trans * glm::vec4(eyeCor, 1.0f));

		OrientLeftByDegree(degree);
	}

	// Rotate camera w.r.t right axis on center coord
	void RotateDownByDegree(float degree)
	{
		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(centerCor));
		trans = glm::rotate(trans, glm::radians(degree), rightVecNorm);
		trans = glm::translate(trans, -glm::vec3(centerCor));

		eyeCor = glm::vec3(trans * glm::vec4(eyeCor, 1.0f));

		OrientUpByDegree(degree);
	}

	// Rotate camera w.r.t right axis on rotate point
	void RotateDownByDegree(float degree, glm::vec3 rotatePoint)
	{
		if (rotatePoint == eyeCor)
			return;

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(rotatePoint));
		trans = glm::rotate(trans, glm::radians(degree), rightVecNorm);
		trans = glm::translate(trans, -glm::vec3(rotatePoint));

		eyeCor = glm::vec3(trans * glm::vec4(eyeCor, 1.0f));

		OrientUpByDegree(degree);
	}

	// Rotate camera w.r.t rotate axis on center coord
	void RotateCounterClockByDegree(float degree, glm::vec3 rotateAxis)
	{
		glm::vec3 rotatePoint = centerCor;

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(rotatePoint));
		trans = glm::rotate(trans, glm::radians(degree), rotateAxis);
		trans = glm::translate(trans, -glm::vec3(rotatePoint));

		eyeCor = glm::vec3(trans * glm::vec4(eyeCor, 1.0f));

		updateCameraVectors();
		viewMatrix = glm::lookAt(eyeCor, centerCor, upVecNorm);
	}

	// Rotate camera w.r.t rotate axis on rotate point
	void RotateCounterClockByDegree(float degree, glm::vec3 rotateAxis, glm::vec3 rotatePoint)
	{
		if (rotatePoint == eyeCor)
			return;

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(rotatePoint));
		trans = glm::rotate(trans, glm::radians(degree), rotateAxis);
		trans = glm::translate(trans, -glm::vec3(rotatePoint));

		eyeCor = glm::vec3(trans * glm::vec4(eyeCor, 1.0f));

		updateCameraVectors();
		viewMatrix = glm::lookAt(eyeCor, centerCor, upVecNorm);
	}

	void ShowStatus()
	{
		std::cout << "\n-----------------" << std::endl;
		std::cout << "eyeCor: " << eyeCor.x << " # " << eyeCor.y << " # " << eyeCor.z << std::endl;
		std::cout << "centerCor: " << centerCor.x << " # " << centerCor.y << " # " << centerCor.z << std::endl;
		std::cout << "upVecNorm: " << upVecNorm.x << " # " << upVecNorm.y << " # " << upVecNorm.z << std::endl;
		std::cout << "lookVecNorm: " << lookVecNorm.x << " # " << lookVecNorm.y << " # " << lookVecNorm.z << std::endl;
		std::cout << "rightVecNorm: " << rightVecNorm.x << " # " << rightVecNorm.y << " # " << rightVecNorm.z << std::endl;
		std::cout << "mod of lookVec: " << glm::dot(lookVec, lookVecNorm) << std::endl;
	}

};