#include "stdafx.h"
#include "camera.h"
#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main()
{
	Camera testCamera = Camera(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 1));
	//Camera testCamera = Camera();
	testCamera.ShowStatus();
	
	testCamera.OrientLeftByDegree(60);
	testCamera.ShowStatus();

	testCamera.RotateRightByDegree(60);
	testCamera.ShowStatus();
	
	std::cin.get();
}