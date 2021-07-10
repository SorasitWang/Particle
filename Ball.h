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
	float acc=0.0f , g=0.04,gVelo = 0.0f;
	glm::vec3 velocity = glm::vec3(0.3f,0.5f,0.0f);
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
		shader.use();

		glm::mat4 model = glm::mat4(1.0f);

		std::vector<float> wallBorder;
		wallBorder.push_back(sizeWall); wallBorder.push_back(-sizeWall); wallBorder.push_back(sizeWall); wallBorder.push_back(-sizeWall);
		//std::cout << isColWall(wallBorder) << std::endl;
		
		
		
		std::string check = isColWall(wallBorder);
		isColBall(balls, idx);

		/*if (check != "")
			std::cout << check << std::endl;*/
		if (check == "UP" || check=="DOWN") {
			velocity.y *= -1;
		}
		else if (check == "RIGHT" || check == "LEFT") {
			velocity.x *= -1;
		}
		position += velocity * deltaTime;
		//gVelo += g * deltaTime;
		//velocity.y -= gVelo * deltaTime;
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
		this->velocity.x = ((float)rand() / RAND_MAX) - 0.5;
		this->velocity.y = ((float)rand() / RAND_MAX) - 0.5;

		this->position.x = (((float)rand() / RAND_MAX) - 0.5) ;
		this->position.y = (((float)rand() / RAND_MAX) - 0.5) ;

		this->velocity = glm::normalize(velocity);
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

	void isColBall(std::vector<Ball> balls, int idx) {
		for (int i = 0; i < balls.size(); i++) {
			if (collapse(balls[i].position) && idx != i) {
				std::cout << "col" << std::endl;
				glm::vec3 tmp = velocity;
				//std::cout << tmp.x << " " << tmp.y << " " << balls[i].velocity.x << " " << balls[i].velocity.y << std::endl;
				this->velocity = glm::vec3(-1) * this->velocity + balls[i].velocity;
				balls[i].velocity = glm::vec3(-1) * balls[i].velocity + tmp;
				//std::cout << this->velocity.x << " " << this->velocity.y << " " << balls[i].velocity.x << " " << balls[i].velocity.y << std::endl;
				this->velocity = glm::normalize(this->velocity);
				balls[i].velocity = glm::normalize(balls[i].velocity);
				return;
			}
		}

	}

	float collapse(glm::vec3 pos2) {
		std::cout << sqrt(pow(this->position.x - pos2.x, 2) + pow(this->position.y - pos2.y, 2)) << std::endl;
		return sqrt(pow(this->position.x - pos2.x, 2) + pow(this->position.y - pos2.y, 2)) <= 2*this->radius;
	}
};