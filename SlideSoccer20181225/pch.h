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
constexpr float GROUND_WIDTH = 75.0f, GROUND_HEIGHT = 0.2f, GROUND_DEPTH = 50.0f;
constexpr float WALL_THICK = 3.0f, WALL_HEIGHT = 5.0f;
constexpr float PITCH_WIDTH = 20.0f, PITCH_HEIGHT = 10.0f, PITCH_DEPTH = 2.0f;

constexpr unsigned int SHADOW_WIDTH = 2048;
constexpr unsigned int SHADOW_HEIGHT = 2048;
constexpr unsigned int SHADOW_MAP_ID = 7;

// Camera
const float CAMERA_SMOOTHMOVING_TIME = 0.2;

constexpr float CAMERA_ZNEAR = 0.1f;
constexpr float CAMERA_ZFAR = 1000.0f;

const glm::vec3 CAMERA_POS_1 = glm::vec3(0.0f, 20.0f, 45.0f);
const glm::vec3 CAMERA_CENTER_1 = glm::vec3(0, -5.0f, 0);
//const glm::vec3 CAMERA_CENTER_1 = glm::vec3(0, 20.0f, 0);
const glm::vec3 CAMERA_UPVECNORM_X = glm::vec3(1, 0, 0);
const glm::vec3 CAMERA_UPVECNORM_Y = glm::vec3(0, 1, 0);
const glm::vec3 CAMERA_UPVECNORM_Z = glm::vec3(0, 0, 1);
const glm::vec3 CAMERA_POS_2 = glm::vec3(0.0f, 40.0f, 0.0f);
constexpr float CAMERA_POS_2_Y = 40.0f;
constexpr float CAMERA_POS_3_Y = 20.0f;
const glm::vec3 CAMERA_CENTER_2 = glm::vec3(0, -5.0f, 0);
const glm::vec3 CAMERA_UPVECNORM_2 = glm::vec3(0, 0, -1);

constexpr float CAMERA_LEAN_OFFSET1 = 20.0f;
constexpr float CAMERA_LEAN_OFFSET2 = 30.0f;

// Particle
constexpr unsigned int PARTICLE_MAX_AMOUNT = 100000;
constexpr float PARTICLE_PER_SECOND = 15000.0f;
const glm::vec3 PARTICLE_GRAVITY = glm::vec3(0, -9.8, 0);
constexpr unsigned int PARTICLE_COLLIDE_NUMBER = 10;
constexpr float PARTICLE_COLLIDE_COOLDOWN = 0.3f;
const glm::vec3 PARTICLE_COLOR_BLUE = glm::vec3(0.2, 0.2, 0.7);
const glm::vec3 PARTICLE_COLOR_RED = glm::vec3(0.7, 0.2, 0.2);
const glm::vec3 PARTICLE_COLOR_GREEN = glm::vec3(0.2, 0.6, 0.2);
const glm::vec3 PARTICLE_COLOR_YELLOW = glm::vec3(0.6, 0.6, 0.2);
constexpr float PARTICLE_LIFE = 4.0f;

// Physics
constexpr float ACCELERATION_BASIC = 30.0f;
constexpr float FRICTION_CONSTANT = 4;
constexpr float FRICTION_LINEAR = 0.32f;
constexpr float GAMEMENU_RANDOM_SPEED = 15.0f;

constexpr float GAME_COOLDOWN_TIME = 4.0f;

const glm::vec3 KICKER_POSITION[6] = {
	glm::vec3(-0.25 * GROUND_WIDTH, 0, 0),
	glm::vec3(-0.15 * GROUND_WIDTH, 0, 0.2 * GROUND_DEPTH),
	glm::vec3(-0.15 * GROUND_WIDTH, 0, -0.2 * GROUND_DEPTH),
	glm::vec3(0.25 * GROUND_WIDTH, 0, 0),
	glm::vec3(0.15 * GROUND_WIDTH, 0, 0.2 * GROUND_DEPTH),
	glm::vec3(0.15 * GROUND_WIDTH, 0, -0.2 * GROUND_DEPTH),
};

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
