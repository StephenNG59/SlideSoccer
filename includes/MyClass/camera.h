#pragma once

#include "pch.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
//#include <MyClass/Object/Object3D.h>


enum CameraStatus {
	IsFree = 0,
	IsSmoothlyMoving = 1,
	IsTracking = 2
};

enum CameraTrackingTarget {
	NoTracking = -1,
	Ball = 0,
	Player1 = 1,
	Player2 = 2
};

class Camera
{
public:
	// Constructor with vectors
	Camera(glm::vec3 eyeCor = glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3 centerCor = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 upVec = glm::vec3(0.0f, 1.0f, 0.0f));

	// Constructor with scaler values
	Camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ);

	// Status
	CameraStatus Status = CameraStatus::IsFree;

	// Coordinates
	glm::vec3 EyeCor;
	glm::vec3 CenterCor;

	// Normal vectors
	glm::vec3 UpVecNorm;
	glm::vec3 lookVecNorm;
	glm::vec3 rightVecNorm;
	// Non-normal vectors
	glm::vec3 lookVec;

	void SmoothlyMoveTo(glm::vec3 destPos, glm::vec3 destCenter, glm::vec3 destUpVec, float totalTime);

	void Update(float dt);

private:
	// Smothly moving
	float movingTotalTime = 2;
	glm::vec3 destPos = CAMERA_POS_1;
	glm::vec3 destCenter = CAMERA_CENTER_1;
	glm::vec3 destUpVecNorm = CAMERA_UPVECNORM_Y;

	// Matrix
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// Perspective
	float fov, aspect, zNear, zFar;			// #TODO make sure to initialize

	// Update vectors and matrix based on eye coord and center coord
	void updateCameraVectors();
	void updateProjectionMatrix();

	// Tracking target
	CameraTrackingTarget target = CameraTrackingTarget::NoTracking;

public:
	float Fov = 45.0f;
	//float TrackCD = 5.0f;	/// prevent the case: translate camera before smoothly moving finish

	void SetPerspective(float fov, float aspect, float zNear, float zFar);

	// Get the eye position
	glm::vec3 GetPosition();

	// Get front direction
	glm::vec3 GetFrontDirection();

	// Get view matrix
	glm::mat4 GetViewMatrix();

	// Get projection matrix
	glm::mat4 GetProjectionMatrix();

	// Go up
	void GoUp(float distance);

	// Go right
	void GoRight(float distance);

	// Go forward
	void GoForward(float distance);

	// Orient camera direction w.r.t up axis
	void OrientLeftByDegree(float degree);

	// Orient camera direction w.r.t right axis
	void OrientUpByDegree(float degree);

	// Rotate camera w.r.t up axis on center coord
	void RotateRightByDegree(float degree);

	// Rotate camera w.r.t up axis on rotate point
	void RotateRightByDegree(float degree, glm::vec3 rotatePoint);

	// Rotate camera w.r.t right axis on center coord
	void RotateDownByDegree(float degree);

	// Rotate camera w.r.t right axis on rotate point
	void RotateDownByDegree(float degree, glm::vec3 rotatePoint);

	// Rotate camera w.r.t rotate axis on center coord
	void RotateCounterClockByDegree(float degree, glm::vec3 rotateAxis);

	// Rotate camera w.r.t rotate axis on rotate point
	void RotateCounterClockByDegree(float degree, glm::vec3 rotateAxis, glm::vec3 rotatePoint);

	// Translate (only suitable for translation, not safe!)
	void TranslateTo(glm::vec3 centerCor);

	/// Set tracking target
	void SetTrackingTarget(CameraTrackingTarget target);
	CameraTrackingTarget GetTarget();

	void ShowStatus();


};


