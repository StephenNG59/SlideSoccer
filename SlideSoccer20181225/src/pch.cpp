// pch.cpp: 与预编译标头对应的源文件；编译成功所必需的

#include "pch.h"

// 一般情况下，忽略此文件，但如果你使用的是预编译标头，请保留它。


float vecMod(glm::vec3 v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float vecMod(glm::vec2 v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

void printVec3(std::string name, glm::vec3 v)
{
	std::cout << name << ": ";
	std::cout << v.x << " # " << v.y << " # " << v.z << std::endl;
}

void printVec3(glm::vec3 v)
{
	std::cout << "vec3: " << v.x << " # " << v.y << " # " << v.z << std::endl;
}

void testPrint(std::string s)
{
	std::cout << s << std::endl;
}

void testPrint(float f)
{
	std::cout << " #" << f << "# " << std::endl;
}


