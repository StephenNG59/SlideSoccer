// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

#ifndef PCH_H
#define PCH_H

// TODO: 添加要在此处预编译的标头

#include <glm\glm.hpp>
#include <string>

// settings
constexpr float CAMERA_ZNEAR = 0.1f;
constexpr float CAMERA_ZFAR = 1000.0f;

constexpr unsigned int SHADOW_WIDTH = 2048;
constexpr unsigned int SHADOW_HEIGHT = 2048;
constexpr unsigned int SHADOW_MAP_ID = 7;

const float CAMERA_SMOOTHMOVING_TIME = 0.2;

const glm::vec3 CAMERA_POS_1 = glm::vec3(0.0f, 20.0f, 45.0f);
const glm::vec3 CAMERA_CENTER_1 = glm::vec3(0, -5.0f, 0);
//const glm::vec3 CAMERA_CENTER_1 = glm::vec3(0, 20.0f, 0);
const glm::vec3 CAMERA_UPVECNORM_X = glm::vec3(1, 0, 0);
const glm::vec3 CAMERA_UPVECNORM_Y = glm::vec3(0, 1, 0);
const glm::vec3 CAMERA_UPVECNORM_Z = glm::vec3(0, 0, 1);
const glm::vec3 CAMERA_POS_2 = glm::vec3(0.0f, 30.0f, 0.0f);
const glm::vec3 CAMERA_CENTER_2 = glm::vec3(0, -5.0f, 0);
const glm::vec3 CAMERA_UPVECNORM_2 = glm::vec3(0, 0, -1);

constexpr unsigned int PARTICLE_COLLIDE_NUMBER = 10;

constexpr float PARTICLE_COLLIDE_COOLDOWN = 0.3f;

constexpr float ACCELERATION_BASIC = 30.0f;

float vecMod(glm::vec3 v);
float vecMod(glm::vec2 v);
void printVec3(std::string name, glm::vec3 v);
void printVec3(glm::vec3 v);
void testPrint(std::string s);
void testPrint(float f);

enum BallStatus {
	BallIsFree = 0,
	Score1 = 1,
	Score2 = 2,
	WaitForReset = 3,
};

struct BallInfo {
	BallStatus Status;
};


#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <string>

#endif //PCH_H
