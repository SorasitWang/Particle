#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "../header/shader_m.h"
#include "../header/stb_image.h"
#include "../header/camera.h"
#include <iostream>
#include <string>
#include <time.h>

glm::vec3 intercept2D(glm::vec3 aVec, glm::vec3 aPos, glm::vec3 bVec,glm::vec3 bPos) {

	float ma = aVec.y / aVec.x;
	float mb = bVec.y / bVec.y;
	float cb = bPos.y - mb * bPos.x;
	float ca = aPos.y - mb * aPos.x;
	float intersection_X = (cb - ca) / (ma - mb);

	float intersection_Y = ma * intersection_X + ca;
	return glm::vec3(intersection_X, intersection_Y, 0.0f);
}

int checkSign(float x) {
	if (x < 0) return -1;
	if (x > 0) return 1;
	else return 0;
}

float notNear0(float value, float offset) {
	if (value <= 0 && value > -offset) return -offset;
	if (value >= 0 && value < offset) return offset;
	return value;
}

void printVec3(glm::vec3 v) {
	std::cout << v.x << " " << v.y << " " << v.z << std::endl;
}