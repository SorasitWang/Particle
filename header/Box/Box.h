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
		this->sizeX = size; this->sizeY = size;
	}
	unsigned int VAO, VBO, EBO;
	float sizeX = 0.5f, sizeY = 0.5f,sizeZ=0.5f;
	float changeX =1 ,changeY = 1;
	float sizeRat = 1;
	float size = 0.5f;
	float thin = 0.03;
	float velocity = 0.1f;
	bool threeD = false;

	void init(Shader shader,bool threeD) {
		this->threeD = threeD;
		float vertices[3*8];
		int index[24];

		vertices[0] = -sizeX;		vertices[1] = -sizeY;		vertices[2] = 0;			//ซ้ายล่าง ใน
		vertices[3] = -sizeX - thin;	vertices[4] = -sizeY - thin;	vertices[5] = 0;			//ซ้ายล่าง นอก

		vertices[6] = -sizeX;		vertices[7] = sizeY;			vertices[8] = 0;			//ซ้ายบน ใน
		vertices[9] = -sizeX - thin;	vertices[10] = sizeY + thin;	vertices[11] = 0;			//ซ้ายบน นอก

		vertices[12] = sizeX;			vertices[13] = -sizeY;		vertices[14] = 0;			//ขวาล่าง ใน
		vertices[15] = sizeX + thin;	vertices[16] = -sizeY - thin;	vertices[17] = 0;			//ขวาล่าง นอก

		vertices[18] = sizeX;			vertices[19] = sizeY;			vertices[20] = 0;			//ขวาบน ใน
		vertices[21] = sizeX + thin;	vertices[22] = sizeY + thin;	vertices[23] = 0;			//ขวาบน นอก

		index[0] = 0;		index[1] = 1;	index[2] = 2;			//ซ้าย
		index[3] = 3;		index[4] = 1;	index[5] = 2;

		index[6] = 2;		index[7] = 3;	index[8] = 6;			//บน
		index[9] = 7;		index[10] = 3;	index[11] = 6;

		index[12] = 4;		index[13] = 5;	index[14] = 6;			//ขวา
		index[15] = 7;		index[16] = 5;	index[17] = 6;

		index[18] = 0;		index[19] = 1;	index[20] = 4;			//ล่าง
		index[21] = 5;		index[22] = 1;	index[23] = 4;
			

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

		shader.use();
		shader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
		shader.setFloat("alpha", 1.0f);

	}

	void draw(Shader shader,glm::mat4 projection, glm::mat4 view) {
	
		shader.use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(sizeRat));
		//model = glm::scale(model, glm::vec3(changeX, changeY, 0.0f));
		//std::cout << changeX << " " << changeY << std::endl;
		shader.setMat4("model", model);
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setBool("threeD", threeD);
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);


		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(sizeRat));
		model = glm::translate(model, glm::vec3(0.5f+thin , 0.0f, 0.5f + thin));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(sizeRat));
		model = glm::translate(model, glm::vec3(-0.5f-thin, 0.0f, 0.5f + thin));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(sizeRat));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f,1.f+thin));
		shader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(sizeRat));
		model = glm::translate(model, glm::vec3(0.0f, 0.5f + thin, 0.5f + thin));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(sizeRat));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f-thin, 0.5f + thin));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		shader.setMat4("model", model);
		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
	}


	void changeSize(float deltaTime,int type,Shader shader) {
		changeX = 0;
		if (type == 0) {
			changeX += deltaTime * velocity;
		}
		if (type == 1) {
			changeX -= deltaTime * velocity;
		}
		if (type == 2) {
			changeY += deltaTime * velocity;
		}
		if (type == 3) {
			changeY -= deltaTime * velocity;
		}
		std::cout << sizeRat << " " <<changeX<< std::endl;
		sizeRat = std::max(0.0f,1+ changeX*1000);
		sizeY = std::max(0.0f, size * changeY);

		//this->init(shader);
	}

};