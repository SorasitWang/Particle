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

class Box {
public :
	Box(float size) {
		this->sizeX = size; this->sizeY = size; this->sizeZ = size;
	}
	unsigned int VAO, VBO, EBO;
	float sizeX = 0.5f, sizeY = 0.5f,sizeZ=0.5f;
	float changeX =1 ,changeY = 1;
	float sizeRat = 1;
	float size = 0.5f;
	float thin = 0.03;
	int n;
	float velocity = 0.1f;
	bool threeD = false;

	void init(Shader shader,bool threeD) {
		this->threeD = threeD;
		float vertices[3*8];

		vertices[0] = -sizeX; vertices[1] = sizeY; vertices[2] = -sizeZ;
		vertices[3] =  sizeX; vertices[4] = sizeY; vertices[5] = -sizeZ;
		vertices[6] =  sizeX; vertices[7] = -sizeY; vertices[8] = -sizeZ;
		vertices[9] = -sizeX; vertices[10] = -sizeY; vertices[11] = -sizeZ;

		vertices[12] = -sizeX; vertices[13] = sizeY; vertices[14] = sizeZ;
		vertices[15] = sizeX; vertices[16] = sizeY; vertices[17] = sizeZ;
		vertices[18] = sizeX; vertices[19] = -sizeY; vertices[20] = sizeZ;
		vertices[21] = -sizeX; vertices[22] = -sizeY; vertices[23] = sizeZ;

		int index[] = {
			0,3,7,4,
			5,1,2,3,
			7,6,2,1,
			0,4,5,6,
			7,4

			
		};
		n = 18;

		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		shader.use();
		shader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setFloat("alpha", 1.0f);

	}

	void draw(Shader shader,glm::mat4 projection, glm::mat4 view) {
	
		shader.use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(sizeRat));

		shader.setMat4("model", model);
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setBool("threeD", threeD);
		glBindVertexArray(this->VAO);
		glLineWidth(3.0f);
		glDrawElements(GL_LINE_LOOP, n, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
	}


	void changeSize(float deltaTime,int type,Shader shader) {
		changeX = 0;
		if (type == 0) {
			sizeX += deltaTime * velocity;
		}
		if (type == 1) {
			sizeX -= deltaTime * velocity;
		}
		if (type == 2) {
			sizeY += deltaTime * velocity;
		}
		if (type == 3) {
			sizeY -= deltaTime * velocity;
		}
		if (type == 4) {
			sizeZ -= deltaTime * velocity;
		}
		if (type == 5) {
			sizeZ += deltaTime * velocity;
		}
	
		this->init(shader,threeD);
	}

};