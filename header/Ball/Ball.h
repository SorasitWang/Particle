#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "../shader_m.h"
#include "../stb_image.h"
#include "../camera.h"
#include <iostream>
#include <string>
#include <time.h>
#include "../Utility.h"
struct ballProperty {
	float velocityLostWall = 0.05f;
	float g = 0.01f;
	float velocityLostBump = 0.0f;
	float lifeTime = 120.0f;
	float friction = 0.01f;
};
struct ColProperty {
	glm::vec3 pos = glm::vec3(4.0, 0.0, 0.0);
	glm::vec3 rotate = glm::vec3(0.0, 0.0, 0.0);
	float countFade = 0.0f;
	bool col = false;
	int axis = -1;
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
	unsigned int colVAO, colVBO, colEBO;
	float radius = 0.1;
	float lifeTime = 500.0f;
	float curTime = 0;
	float veloLostWall =0.95;
	float veloLostBump = 0.0f;
	float friction = 0.01;
	float g=0.01,gVelo = 0.0f;
	int countOnGround = 0;
	int countOnRoof = 0;
	bool isIn = true;
	bool move = true;
	bool col = false;
	float colFade = 0.5f;
	std::vector<ColProperty> colMarkers;
	bool threeD = false;
	glm::vec3 velocity = glm::vec3(1, 1, 1);
	glm::vec3 direction = glm::vec3(0.3,0.5,0.0f);
	glm::vec3 color = glm::vec3(0.8f, 0.3f, 0.3f);
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

		

		float colVertices[360 / 5 * 3 + 3];
		int colIndex[73 * 3];
		int idx = 3;
		colVertices[0] = 0.0f; colVertices[1] = 0.0f; colVertices[2] = 0.0f;
		for (int i = 0; i < 360; i += 5) {
			colVertices[idx] = radius * glm::cos(glm::radians((float)i)); colVertices[idx + 1] = radius * glm::sin(glm::radians((float)i)); colVertices[idx + 2] = 0.0f;
			idx += 3;
		}
		idx = 0;
		for (int i = 1; i <= 72; i++) {
			colIndex[idx] = 0; colIndex[idx + 1] = i; colIndex[idx + 2] = (i + 1) > 72? 1 :i+1;
			idx += 3;
		}
		/*float size = 0.5;	
		float colVertices[] = {
		  size, -0.5f, size,     
		  size, -0.5f, -size,   
		 -size, -0.5f,-size,    
		 -size, -0.5f, size,   
		};

		int colIndex[] = {
			0,1,3,
			1,2,3
		};*/
		glGenVertexArrays(1, &this->colVAO);
		glGenBuffers(1, &this->colVBO);
		glGenBuffers(1, &this->colEBO);
		//เชื่อม


		glBindVertexArray(this->colVAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->colVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colVertices), colVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->colEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(colIndex), colIndex, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		
	}

	void draw(Shader shader,Shader colShader,float deltaTime,float sizeWallX,float sizeWallY,float sizeWallZ,
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
			if (isTouchSurface(wallBorder))
				velocity.x -= friction * deltaTime;
			float p;
			
			colShader.use();
			
			int c = findAvaMarker();
			
			switch (isColWall(wallBorder, p))
			{
			case 0 :{
				
				colMarkers[c].pos = glm::vec3(p, position.y, position.z);
				colMarkers[c].col = true;
				colMarkers[c].rotate = glm::vec3(0.0, 1.0, 0.0);
				colMarkers[c].axis = 0;
				//std::cout << "x" << std::endl;
				//colShader.setInt("axis", 0);
				break;
				}
			case 1: {
				colMarkers[c].pos = glm::vec3(position.x, p, position.z);
				colMarkers[c].col = true;
				colMarkers[c].rotate = glm::vec3(1.0, 0.0, 0.0);
				//std::cout << "y" << std::endl;
				colMarkers[c].axis = 1;
				//colShader.setInt("axis", 1);
				break;
			}
			case 2: {
				colMarkers[c].pos = glm::vec3(position.x, position.y, p);
				colMarkers[c].col = true;
				colMarkers[c].rotate = glm::vec3(0.0, 0.0, 1.0);
				//std::cout << "z" << std::endl;
				colMarkers[c].axis = 2;
				
				break;
			}
			//default: //col = false;
			}
			
			velocity.x = std::max(0.0f, velocity.x);
			position.y += velocity.y * deltaTime;
			position.x += direction.x * velocity.x * deltaTime;
			if(threeD) position.z += direction.z * velocity.z * deltaTime;
			gVelo += g * deltaTime;
			velocity.y -= gVelo * deltaTime;
			direction.y -= gVelo;
		}
		shader.use();
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

		for (auto &marker:colMarkers){
			
			if (marker.col && threeD && marker.pos.x != 4.0f) {
				if (marker.countFade > colFade) {
					marker.countFade = 0.0f;
					marker.col = false;
					//std::cout << "gone" << std::endl;
					continue;
				}
				//printVec3(marker.rotate);
				switch (marker.axis) {
				case 0:
					marker.pos.x = copysign( sizeWallX, marker.pos.x);
					break;
				case 1 :
					marker.pos.y = copysign(sizeWallY, marker.pos.y);
					break;
				case 2 :
					marker.pos.z = copysign( sizeWallZ,marker.pos.z);
					break;

				}
				marker.countFade += deltaTime;
				colShader.use();
				colShader.setInt("axis", marker.axis);
				colShader.setFloat("time", 1 - marker.countFade / colFade);
				model = glm::mat4(1.0f);
				model = glm::translate(model, marker.pos);
				model = glm::rotate(model, glm::radians(90.0f), marker.rotate);
				//model = glm::scale(model,glm::vec3(10.0f) );

				colShader.setMat4("model", model);
				colShader.setMat4("projection", projection);
				colShader.setMat4("view", view);
				colShader.setVec3("color", color);

				colShader.setFloat("rad", radius);
				colShader.setVec3("center", marker.pos);
				/*glBindVertexArray(this->VAO);
				//glDrawArrays(GL_TRIANGLES, 0, 3);
				//glDrawElements(GL_TRIANGLES, 73*3, GL_UNSIGNED_INT, 0);
				glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);*/

				glBindVertexArray(colVAO);
				//glDrawArrays(GL_TRIANGLES, 0, 3);
				glDrawElements(GL_TRIANGLES, 73 * 3, GL_UNSIGNED_INT, 0);
				//glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				//printVec3(colPos);
			}
		}
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

	int isColWall(std::vector<float> wallBorder,float &p) {
		//std::cout << 
		float x = wallBorder[0], y = wallBorder[1], z = wallBorder[2];
		//std::cout << position.x+radius << " " << right << std::endl;
		if (isIn == false) {
			
			if (position.y + radius <= y && position.x - radius >= -x 
				&& position.x + radius <= x && position.y - radius >= -y) {
				std::cout << "In" << std::endl;
				isIn = true;
			}

			return -1;
		}
		if (position.x + radius >= x) {
			position.x = x - radius;
			direction.x *= -1;
			velocity.x *= veloLostWall;
			p = x;
			return 0;
		}
		if (position.x - radius <= -x) {
			position.x = -x + radius;
			direction.x *= -1;
			velocity.x *= veloLostWall;
			p = -x;
			return 0;
		}
		if (position.y + radius >= y) {
			position.y = y - radius;
			velocity.y *= -veloLostWall;
			p = y;
			return 1;
		}
		if (position.y - radius <= -y) {
			position.y = -y + radius;
			velocity.y *= -veloLostWall;
			p = -y;
			return 1;
		}

			if (position.z - radius <= -z) {
				position.z = -z + radius;
				direction.z *= -1;
				velocity.z *= veloLostWall;
				p = -z;
				return 2;
			}
			if (position.z + radius >= z) {
				position.z = z - radius;
				direction.z *= -1;
				velocity.z *= veloLostWall;
				p = z;
				return 2;
			}
		
		return -1;
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

	bool isTouchSurface(std::vector<float> wallBorder) {
		float threshold = 0.001;
		float down= -wallBorder[1] , up = wallBorder[1];
		if (abs(this->position.y - this->radius - down) < threshold)
			countOnGround += 1;
		else countOnGround = 0;
		if (countOnGround > 100) return true;
		
		if (abs(this->position.y + this->radius - up) < threshold)
			countOnRoof += 1;
		else countOnRoof = 0;
		if (countOnRoof > 100) return true;
	
		return false;
	}

	int findAvaMarker() {
		for (int i = 0; i < colMarkers.size(); i++) {
			if (colMarkers[i].col == false) {
				return i;
			}
		}
		colMarkers.push_back(ColProperty());
		return colMarkers.size() - 1;
	}
	
};