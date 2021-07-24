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
struct ballProperty {
	float velocityLostWall = 0.05f;
	float g = 0.01f;
	float velocityLostBump = 0.0f;
	float lifeTime = 120.0f;
	float friction = 0.01f;
};
const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;
const GLfloat PI = 3.14159265358979323846f;
class Ball {
public:
	Ball(float g) {
		this->g = g;
	};
	Ball() {

	};
	std::vector<glm::vec3> allColor = { glm::vec3(0.8f, 0.3f, 0.3f) , glm::vec3(0.3f, 0.8f, 0.3f) , glm::vec3(0.3f, 0.3f, 0.8f),
										glm::vec3(0.6f, 0.6f, 0.2f) ,glm::vec3(0.6f, 0.2f, 0.6f) ,glm::vec3(0.2f, 0.6f, 0.6f) };
	unsigned int VAO, VBO, EBO;
	float radius = 0.07;
	float lifeTime = 500.0f;
	float curTime = 0;
	float veloLostWall =0.95;
	float veloLostBump = 0.0f;
	float friction = 0.01;
	float g=0.01,gVelo = 0.0f;
	int countOnGround = 0;
	bool isIn = true;
	bool move = true;
	bool threeD = false;
	glm::vec3 velocity = glm::vec3(1, 1, 1);
	glm::vec3 direction = glm::vec3(0.3,0.5,0.0f);
	glm::vec3 color = glm::vec3(0.8f, 0.3f, 0.3f);
	//glm::vec3 velocity = glm::vec3(1,1,0);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);



	void init(Shader shader, ballProperty prop,bool threeD,std::vector<Ball> balls) {
		this->threeD = threeD;
		lifeTime = prop.lifeTime;
		g = prop.g;
		veloLostWall = 1 - prop.velocityLostWall;
		veloLostBump = 1 - prop.velocityLostBump;
		friction = prop.friction;

		std::vector<float> sphereVertices;
		std::vector<int> sphereIndices;

		/*2-Calculate the vertices of the sphere*/
		//Generate the vertices of the ball
		for (int y = 0; y <= Y_SEGMENTS; y++)
		{
			for (int x = 0; x <= X_SEGMENTS; x++)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				sphereVertices.push_back(xPos);
				sphereVertices.push_back(yPos);
				sphereVertices.push_back(zPos);
			}
		}

		//Indices that generate the ball
		for (int i = 0; i < Y_SEGMENTS; i++)
		{
			for (int j = 0; j < X_SEGMENTS; j++)
			{
				sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
				sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
				sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
				sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
				sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
				sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
			}
		}


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
		
		setRand(balls);
		
		
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);
		//เชื่อม


		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		
	}

	void draw(Shader shader,float deltaTime,float sizeWallX,float sizeWallY,float sizeWallZ,
				std::vector<Ball> balls,int idx,glm::mat4 projection,glm::mat4 view,ballProperty prop) {
		
		friction = prop.friction;
		g = prop.g;
		veloLostWall = 1 - prop.velocityLostWall;
		veloLostBump = 1 - prop.velocityLostBump;
		
		curTime += deltaTime;
		if (curTime > lifeTime) return;
		shader.use();

		glm::mat4 model = glm::mat4(1.0f);

		std::vector<float> wallBorder;
		wallBorder.push_back(sizeWallX); wallBorder.push_back(sizeWallY); wallBorder.push_back(sizeWallZ);
		//std::cout << isColWall(wallBorder) << std::endl;
		
		
		
		
		isColBall(balls, idx);
		//std::cout << direction.y << std::endl;
		/*if (check == "UP" || check=="DOWN") {
			//direction.y *= -1;
			velocity.y *= -veloLost ;
		}
		else if (check == "RIGHT" || check == "LEFT") {
			direction.x *= -1;
			velocity.x *= 0.8;
		}*/

		
		if (move == true) {
			if (isOnGround(wallBorder))
				velocity.x -= friction * deltaTime;

			isColWall(wallBorder);

			velocity.x = std::max(0.0f, velocity.x);
			position.y += velocity.y * deltaTime;
			position.x += direction.x * velocity.x * deltaTime;
			if(threeD) position.z += direction.z * velocity.z * deltaTime;
			gVelo += g * deltaTime;
			velocity.y -= gVelo * deltaTime;
			direction.y -= gVelo;
		}
		model = glm::translate(model, glm::vec3(position.x,position.y,position.z*threeD));
		model = glm::scale(model, glm::vec3(radius));
		
		shader.setMat4("model", model);
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setBool("threeD", threeD);
		shader.setVec3("color", color);
		//std::cout << 1.0f - (curTime / lifeTime) << std::endl;
		shader.setFloat("alpha", 1.0f-(curTime/ lifeTime));
		glBindVertexArray(this->VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawElements(GL_TRIANGLES, 73*3, GL_UNSIGNED_INT, 0);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

	

private :

	void setRand(std::vector<Ball> balls) {
		do {
			this->position.x = glm::clamp(((float)rand() / RAND_MAX) - 0.5, -0.485, 0.485);
			this->position.y = glm::clamp(((float)rand() / RAND_MAX) - 0.5, -0.485, 0.485);
			this->position.z = glm::clamp(((float)rand() / RAND_MAX) - 0.5, -0.485, 0.485);
		} while (isColBall(balls, balls.size() - 1));
		rand();
		this->direction.x = ((float)rand() / RAND_MAX) - 0.5;
		this->direction.y = ((float)rand() / RAND_MAX) - 0.5;
		this->direction.z = ((float)rand() / RAND_MAX) - 0.5;
		//this->direction = glm::normalize(this->direction);

		color = allColor[rand() % allColor.size()];

		

		this->direction = glm::normalize(direction);
		//this->directionX = this->velocity.x;
		//this->velocity.x = 1;
	}

	std::string isColWall(std::vector<float> wallBorder) {
		//std::cout << 
		float x = wallBorder[0], y = wallBorder[1], z = wallBorder[2];
		//std::cout << position.x+radius << " " << right << std::endl;
		if (isIn == false) {
			
			if (position.y + radius <= y && position.x - radius >= -x 
				&& position.x + radius <= x && position.y - radius >= -y) {
				std::cout << "In" << std::endl;
				isIn = true;
			}

			return "";
		}
		if (position.x + radius >= x) {
			position.x = x - radius;
			direction.x *= -1;
			velocity.x *= veloLostWall;
			//return "RIGHT";
		}
		if (position.x - radius <= -x) {
			position.x = -x + radius;
			direction.x *= -1;
			velocity.x *= veloLostWall;
			//return "LEFT";
		}
		if (position.y + radius >= y) {
			position.y = y - radius;
			velocity.y *= -veloLostWall;
			
			//return "UP";
		}
		if (position.y - radius <= -y) {
			position.y = -y + radius;
			velocity.y *= -veloLostWall;
			//return "DOWN";
		}

			if (position.z - radius <= -z) {
				position.z = -z + radius;
				direction.z *= -1;
				velocity.z *= veloLostWall;
			}
			if (position.z + radius >= z) {
				position.z = z - radius;
				direction.z *= -1;
				velocity.z *= veloLostWall;
				//return "LET";
			}
		
		return "";
	}

	bool isColBall(std::vector<Ball> balls, int idx) {
		
		bool re = false;
		for (int i = 0; i < balls.size(); i++) {
			if (collapse(balls[i].position) && idx != i && balls[i].curTime <= balls[i].lifeTime) {
				//std::cout << "this " << position.x << " " << position.y << " / "<<direction.x << " " << velocity.y <<std::endl;
				//std::cout << "ball " << balls[i].position.x << " " << balls[i].position.y << " / " << balls[i].direction.x << " " << balls[i].velocity.y << std::endl;
				glm::vec3 link = glm::normalize(glm::vec3(this->position.x- balls[i].position.x , this->position.y - balls[i].position.y, this->position.z - balls[i].position.z));
				glm::vec3 normal = glm::normalize(glm::cross(link, glm::vec3(0.0f, 0.0f, 1.0f)));


				
				glm::vec3 tmpA = glm::normalize(glm::vec3(this->direction.x, this->velocity.y, this->direction.z*threeD));
				glm::vec3 tmpB = glm::normalize(glm::vec3(balls[i].direction.x, balls[i].velocity.y, balls[i].direction.z * threeD));

				//this
				//std::cout << veloLost << std::endl;
				glm::vec3 reflect = glm::normalize(position- balls[i].position);
				float sizeThis = direction.x * direction.x + velocity.y * velocity.y + direction.z * direction.z * threeD;
				this->direction.x = (reflect.x + 1*direction.x);
				this->velocity.y =  (reflect.y + 1*velocity.y);
				if (threeD) this->direction.z = (reflect.z + direction.z);
				float ratio = (direction.x * direction.x + velocity.y * velocity.y + direction.z*direction.z*threeD)/sizeThis;
				this->direction.x /= sqrt(ratio / veloLostBump);
				this->velocity.y /=  sqrt(ratio / veloLostBump);
				if (threeD) this->direction.z /= sqrt(ratio);

				//another
				float sizeAnother = balls[i].direction.x * balls[i].direction.x + balls[i].velocity.y * balls[i].velocity.y 
									+ balls[i].direction.z * balls[i].direction.z * threeD;
				balls[i].direction.x = (-reflect.x + 1 * balls[i].direction.x);
				balls[i].velocity.y = (-reflect.y + 1*balls[i].velocity.y);
				if (threeD)balls[i].direction.z = (-reflect.z + 1 * balls[i].direction.z);
				ratio = (balls[i].direction.x * balls[i].direction.x + balls[i].velocity.y * balls[i].velocity.y
						+ balls[i].direction.z * balls[i].direction.z * threeD) / sizeAnother;
				balls[i].direction.x /= sqrt(ratio / veloLostBump);
				balls[i].velocity.y /= sqrt(ratio / veloLostBump);
				if (threeD) balls[i].direction.z /= sqrt(ratio);

				re = true;
			}
		}
		return re;

	}

	float collapse(glm::vec3 pos2) {
		//std::cout << sqrt(pow(this->position.x - pos2.x, 2) + pow(this->position.y - pos2.y, 2)) << std::endl;
		return sqrt(pow(this->position.x - pos2.x, 2) + pow(this->position.y - pos2.y, 2) + pow(this->position.z-pos2.z,2)*threeD )<= 2.0*this->radius;
	}

	bool isOnGround(std::vector<float> wallBorder) {
		float threshold = 0.001;
		float down= -wallBorder[1];
		if (abs(this->position.y - this->radius - down) < threshold)
			countOnGround += 1;
		else countOnGround = 0;
		if (countOnGround > 100) return true;
	
		return false;
	}

	
};