#include "pch.h"
#include "Camera.h"


Camera::Camera(glm::vec3 eyeCor /*= glm::vec3(0.0f, 0.0f, 10.0f)*/, glm::vec3 centerCor /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, glm::vec3 upVec /*= glm::vec3(0.0f, 1.0f, 0.0f)*/) : EyeCor(eyeCor), CenterCor(centerCor), UpVecNorm(upVec)
{
	updateCameraVectors();
}

Camera::Camera(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
{
	EyeCor = glm::vec3(eyeX, eyeY, eyeZ);
	CenterCor = glm::vec3(centerX, centerY, centerZ);
	UpVecNorm = glm::vec3(upX, upY, upZ);
	updateCameraVectors();
}

void Camera::SmoothlyMoveTo(glm::vec3 destPos, glm::vec3 destCenter, glm::vec3 destUpVec, float totalTime)
{
	PreviousStatus = Status;
	Status = CameraStatus::IsSmoothlyMoving;
	movingTotalTime = totalTime;

	this->destPos = destPos;
	this->destCenter = destCenter;
	this->destUpVecNorm = glm::normalize(destUpVec);	// TODO: not safe!!
}

void Camera::Update(float dt)
{
	if (Status == CameraStatus::IsSmoothlyMoving)
	{
		float delta = std::min(abs(dt / movingTotalTime), 1.0f);
		glm::vec3 dPos = destPos - EyeCor;
		glm::vec3 dCenter = destCenter - CenterCor;
		glm::vec3 dUpVecNorm = destUpVecNorm - UpVecNorm;
		//testPrint(vecMod(dUpVecNorm));
		if (vecMod(dPos) < 0.05 && vecMod(dCenter) < 0.05/* && vecMod(dUpVecNorm) < 0.02*/)
		{
			EyeCor = destPos;
			CenterCor = destCenter;
			UpVecNorm = destUpVecNorm;
			updateCameraVectors();
			Status = PreviousStatus;
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
}

void Camera::updateCameraVectors()
{
	lookVec = CenterCor - EyeCor;
	lookVecNorm = glm::normalize(lookVec);
	rightVecNorm = glm::normalize(glm::cross(lookVecNorm, UpVecNorm));
	UpVecNorm = glm::normalize(glm::cross(rightVecNorm, lookVecNorm));

	viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
}

void Camera::updateProjectionMatrix()
{
	projectionMatrix = glm::perspective(fov, aspect, zNear, zFar);
}

void Camera::SetPerspective(float fov, float aspect, float zNear, float zFar)
{
	this->fov = fov;
	this->aspect = aspect;
	this->zNear = zNear;
	this->zFar = zFar;
	updateProjectionMatrix();
}

glm::vec3 Camera::GetPosition()
{
	return EyeCor;
}

glm::vec3 Camera::GetFrontDirection()
{
	return lookVecNorm;
}

glm::mat4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::GoUp(float distance)
{
	EyeCor += distance * UpVecNorm;
	CenterCor += distance * UpVecNorm;
}

void Camera::GoRight(float distance)
{
	EyeCor += distance * rightVecNorm;
	CenterCor += distance * rightVecNorm;
	viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
}

void Camera::GoForward(float distance)
{
	EyeCor += distance * lookVecNorm;
	CenterCor += distance * lookVecNorm;
	viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
}

void Camera::OrientLeftByDegree(float degree)
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

void Camera::OrientUpByDegree(float degree)
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

void Camera::RotateRightByDegree(float degree)
{
	glm::mat4 trans;
	trans = glm::translate(trans, glm::vec3(CenterCor));
	trans = glm::rotate(trans, glm::radians(degree), UpVecNorm);
	trans = glm::translate(trans, -glm::vec3(CenterCor));

	EyeCor = glm::vec3(trans * glm::vec4(EyeCor, 1.0f));

	OrientLeftByDegree(degree);
}

void Camera::RotateRightByDegree(float degree, glm::vec3 rotatePoint)
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

void Camera::RotateDownByDegree(float degree)
{
	glm::mat4 trans;
	trans = glm::translate(trans, glm::vec3(CenterCor));
	trans = glm::rotate(trans, glm::radians(degree), rightVecNorm);
	trans = glm::translate(trans, -glm::vec3(CenterCor));

	EyeCor = glm::vec3(trans * glm::vec4(EyeCor, 1.0f));

	OrientUpByDegree(degree);
}

void Camera::RotateDownByDegree(float degree, glm::vec3 rotatePoint)
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

void Camera::RotateCounterClockByDegree(float degree, glm::vec3 rotateAxis)
{
	glm::vec3 rotatePoint = CenterCor;

	glm::mat4 trans;
	trans = glm::translate(trans, glm::vec3(rotatePoint));
	trans = glm::rotate(trans, glm::radians(degree), rotateAxis);
	trans = glm::translate(trans, -glm::vec3(rotatePoint));

	EyeCor = glm::vec3(trans * glm::vec4(EyeCor, 1.0f));
	UpVecNorm = glm::vec3(trans * glm::vec4(UpVecNorm, 1.0f));

	updateCameraVectors();
	viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
}

void Camera::RotateCounterClockByDegree(float degree, glm::vec3 rotateAxis, glm::vec3 rotatePoint)
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

void Camera::TranslateTo(glm::vec3 centerCor)
{
	glm::vec3 dPos = centerCor - this->CenterCor;
	CenterCor = centerCor;
	this->EyeCor += dPos;
	viewMatrix = glm::lookAt(EyeCor, CenterCor, UpVecNorm);
}


void Camera::SetTrackingTarget(CameraTrackingTarget target)
{
	if (Status == CameraStatus::IsSmoothlyMoving) return;

	this->target = target;

	if (target == CameraTrackingTarget::NoTracking)
	{
		Status = CameraStatus::CameraIsFree;
	}
	else
	{
		Status = CameraStatus::IsTracking;
	}
}

CameraTrackingTarget Camera::GetTarget()
{
	return target;
}

//
//void Camera::SetTrackingTarget(Object3D * target)
//{
//
//}

void Camera::ShowStatus()
{
	std::cout << "\n-----------------" << std::endl;
	std::cout << "eyeCor: " << EyeCor.x << " # " << EyeCor.y << " # " << EyeCor.z << std::endl;
	std::cout << "centerCor: " << CenterCor.x << " # " << CenterCor.y << " # " << CenterCor.z << std::endl;
	std::cout << "upVecNorm: " << UpVecNorm.x << " # " << UpVecNorm.y << " # " << UpVecNorm.z << std::endl;
	std::cout << "lookVecNorm: " << lookVecNorm.x << " # " << lookVecNorm.y << " # " << lookVecNorm.z << std::endl;
	std::cout << "rightVecNorm: " << rightVecNorm.x << " # " << rightVecNorm.y << " # " << rightVecNorm.z << std::endl;
	std::cout << "mod of lookVec: " << glm::dot(lookVec, lookVecNorm) << std::endl;
}

