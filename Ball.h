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

class Ball {
public:
	Ball() {

	}
	unsigned int VAO, VBO, EBO;
	float radius = 0.1;

	void init(Shader shader) {
		float vertices[360/5*3 + 3];
		int index[73*3],idx=3;
		vertices[0] = 0.0f; vertices[1] = 0.0f; vertices[2] = 0.0f;
		for (int i = 0; i < 360; i += 5) {
			vertices[idx] = radius*glm::cos(glm::radians((float)i)); vertices[idx+1] = radius * glm::sin(glm::radians((float)i)); vertices[idx+2] =0.0f ;
			idx += 3;
		}
		idx = 1;
		for (int i = 1; i <= 72; i++) {
			index[idx] = 0; index[idx + 1] = i; index[idx + 2] = (i + 1)%72;
			idx += 3;
		}
		
		std::cout << "ss" << std::endl;

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
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glDrawElements(GL_TRIANGLES, 73*3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}


};