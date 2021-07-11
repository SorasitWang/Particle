#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "./header/shader_m.h"
#include "./header/stb_image.h"
#include "./header/camera.h"
#include <iostream>
#include <string>
#include <time.h>
#include "Utility.h"
class Ball {
public:
	Ball(float g) {
		this->g = g;
	};
	Ball() {

	};
	unsigned int VAO, VBO, EBO;
	float radius = 0.05;
	float lifeTime = 5.0f;
	float curTime = 0;
	float veloLost = 0.95;
	float acc=0.0f , g=0.04,gVelo = 0.0f;
	int countOnGround = 0;
	glm::vec3 velocity = glm::vec3(1, 1, 1);
	glm::vec3 direction = glm::vec3(0.3,0.5,0.0f);
	//glm::vec3 velocity = glm::vec3(1,1,0);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

	void init(Shader shader) {
		float vertices[360/5*3 + 3];
		int index[73*3],idx=3;
		vertices[0] = 0.0f; vertices[1] = 0.0f; vertices[2] = 0.0f;
		for (int i = 0; i < 360; i += 5) {
			vertices[idx] = radius*glm::cos(glm::radians((float)i)); vertices[idx+1] = radius * glm::sin(glm::radians((float)i)); vertices[idx+2] =0.0f ;
			idx += 3;
		}
		idx = 0;
		for (int i = 1; i <= 72; i++) {
			index[idx] = 0; index[idx + 1] = i; index[idx + 2] = (i + 1) > 72 ? 1 : i + 1;
			idx += 3;
		}
		
		setRand();
		
		
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);
		//เชื่อม


		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		
	}

	void draw(Shader shader,float deltaTime,float sizeWall,std::vector<Ball> balls,int idx) {
		curTime += deltaTime;
		//if (curTime > lifeTime) return;
		shader.use();

		glm::mat4 model = glm::mat4(1.0f);

		std::vector<float> wallBorder;
		wallBorder.push_back(sizeWall); wallBorder.push_back(-sizeWall); wallBorder.push_back(sizeWall); wallBorder.push_back(-sizeWall);
		//std::cout << isColWall(wallBorder) << std::endl;
		
		
		
		std::string check = isColWall(wallBorder);
		isColBall(balls, idx,deltaTime);
		//std::cout << direction.y << std::endl;
		if (check == "UP" || check=="DOWN") {
			//direction.y *= -1;
			velocity.y *= -veloLost ;
		}
		else if (check == "RIGHT" || check == "LEFT") {
			direction.x *= -1;
			velocity.x *= 0.8;
		}

		if (isOnGround(wallBorder))
			velocity.x -= 0.01 * deltaTime;
		velocity.x = std::max(0.0f, velocity.x);
		position.y += velocity.y* deltaTime;
		position.x += direction.x * velocity.x * deltaTime;
		gVelo += g * deltaTime;
		velocity.y -= gVelo * deltaTime;
		direction.y -= gVelo;
		velocity += acc * deltaTime;
		model = glm::translate(model, position);
		
		
		shader.setMat4("model", model);

		glBindVertexArray(this->VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 73*3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

	

private :

	void setRand() {
		rand();
		this->direction.x = ((float)rand() / RAND_MAX) - 0.5;
		this->direction.y = ((float)rand() / RAND_MAX) - 0.5;
		
		//this->direction = glm::normalize(this->direction);


		this->position.x = glm::clamp(((float)rand() / RAND_MAX) - 0.5, -0.485, 0.485);
		this->position.y = glm::clamp(((float)rand() / RAND_MAX) - 0.5, -0.485, 0.485);

		this->direction = glm::normalize(direction);
		//this->directionX = this->velocity.x;
		//this->velocity.x = 1;
	}

	std::string isColWall(std::vector<float> wallBorder) {
		float right = wallBorder[0], left = wallBorder[1], up = wallBorder[2], down = wallBorder[3];
		//std::cout << position.x+radius << " " << right << std::endl;
		if (position.x + radius >= right) {
			position.x = right - radius;
			return "RIGHT";
		}
		if (position.x - radius <= left) {
			position.x = left + radius;
			return "LEFT";
		}
		if (position.y + radius >= up) {
			position.y = up - radius;
			return "UP";
		}
		if (position.y - radius <= down) {
			position.y = down + radius;
			return "DOWN";
		}
		return "";
	}

	void isColBall(std::vector<Ball> balls, int idx,float deltaTime) {
		for (int i = 0; i < balls.size(); i++) {
			if (collapse(balls[i].position) && idx != i) {

				glm::vec3 normal = glm::normalize(glm::vec3(this->position.x- balls[i].position.x , this->position.y - balls[i].position.y,0.0f));
				
				/*if vector direction WILL intercept with middle plane => apply bounce
				when use center of ball as origin 
				if intercept point in qurant which associater with vector Ex (+,-):(+,-)  ,  (+,0):(+,0)*/
					//bounce is reflect with normal
					
				/*else if ALREADY interept => apply collision
				if intercept point in qurant which oppoite with vector Ex (+, -):(-, +)  ,  (+, 0):(-, 0)*/
					//collision is combined both vector by add Or reflect in some ways

				//else if never intercept (pararelle) => do nothing
				
				
				std::cout << glm::reflect(this->velocity, glm::vec3(-1) * normal).x << " "<<glm::reflect(this->velocity,  normal).x << std::endl;
				this->velocity = glm::reflect(this->velocity, glm::vec3(-1) * normal);// + glm::vec3(0.8)*balls[i].velocity;
				balls[i].velocity = glm::reflect(balls[i].velocity,normal); //+glm::vec3(0.8) * tmp;
				
				
				//this->velocity
				this->velocity = glm::normalize(this->velocity);
				balls[i].velocity = glm::normalize(balls[i].velocity);

				/*this->position += glm::vec3(3)*this->velocity * deltaTime;
				balls[i].position += glm::vec3(3) * balls[i].velocity * deltaTime;*/
				/*while (collapse(balls[i].position)) {
					this->position += this->velocity * deltaTime;
					balls[i].position += balls[i].velocity * deltaTime;
				}*/
				//return;
			}
		}

	}

	float collapse(glm::vec3 pos2) {
		//std::cout << sqrt(pow(this->position.x - pos2.x, 2) + pow(this->position.y - pos2.y, 2)) << std::endl;
		return sqrt(pow(this->position.x - pos2.x, 2) + pow(this->position.y - pos2.y, 2)) <= 2.0*this->radius;
	}

	bool isOnGround(std::vector<float> wallBorder) {
		float threshold = 0.001;
		float down = wallBorder[3];
		if (abs(this->position.y - this->radius - down) < threshold)
			countOnGround += 1;
		else countOnGround = 0;
		if (countOnGround > 100) return true;
	
		return false;
	}

	
};