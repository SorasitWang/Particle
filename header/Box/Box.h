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
		this->size = size;
	}
	unsigned int VAO, VBO, EBO;
	float size = 0.5;
	float thin = 0.03;
	void init(Shader shader) {
		float vertices[3*8];
		int index[24];

		vertices[0] = -size;		vertices[1] = -size;		vertices[2] = 0;			//ซ้ายล่าง ใน
		vertices[3] = -size - thin;	vertices[4] = -size - thin;	vertices[5] = 0;			//ซ้ายล่าง นอก

		vertices[6] = -size;		vertices[7] = size;			vertices[8] = 0;			//ซ้ายบน ใน
		vertices[9] = -size - thin;	vertices[10] = size + thin;	vertices[11] = 0;			//ซ้ายบน นอก

		vertices[12] = size;			vertices[13] = -size;		vertices[14] = 0;			//ขวาล่าง ใน
		vertices[15] = size + thin;	vertices[16] = -size - thin;	vertices[17] = 0;			//ขวาล่าง นอก

		vertices[18] = size;			vertices[19] = size;			vertices[20] = 0;			//ขวาบน ใน
		vertices[21] = size + thin;	vertices[22] = size + thin;	vertices[23] = 0;			//ขวาบน นอก

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

	}

	void draw(Shader shader) {
		shader.use();

		glm::mat4 model = glm::mat4(1.0f);
	

		shader.setMat4("model", model);
		
		glBindVertexArray(this->VAO);

		glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
	}


};