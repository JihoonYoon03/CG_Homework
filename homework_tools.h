#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

struct ColoredVertex {
	glm::vec3 pos;
	glm::vec3 color;
};

class Cube {
	ColoredVertex vertices[8] =
	{
		// 좌하 앞(0), 우하 앞(1), 우상 앞(2), 좌상 앞(3)
		{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } },
		{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },

		// 좌하 뒤(4), 우하 뒤(5), 우상 뒤(6), 좌상 뒤(7)
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } }
	};
	GLuint indices[36] =
	{
		// 앞면
		0, 1, 2,
		0, 2, 3,
		// 왼쪽면
		4, 0, 3,
		4, 3, 7,
		// 윗면
		3, 2, 6,
		3, 6, 7,
		// 오른쪽면
		1, 5, 6,
		1, 6, 2,
		// 아랫면
		4, 5, 1,
		4, 1, 0,
		// 뒷면
		5, 4, 7,
		5, 7, 6
	};

	glm::vec3 toOrigin[6] = {
		glm::vec3(0.0f, -0.5f, -0.5f),
		glm::vec3(0.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, -0.5f, 0.0f),
		glm::vec3(-0.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.5f)
	};

	GLuint VAO, VBO, EBO;

public:
	Cube();
	void Render();
};

class DisplayBasis;

class DisplayBasis {
	glm::vec3 origin;
	ColoredVertex xyz[3][2];

	GLuint VAO, VBO;
public:
	DisplayBasis(GLfloat offset = 1.0f, const glm::vec3& origin = { 0.0f, 0.0f, 0.0f });
	void Render();
};

std::string read_file(const std::string& filename);
void make_vertexShaders(GLuint& vertexShader, const std::string& shaderName);
void make_fragmentShaders(GLuint& fragmentShader, const std::string& shaderName);
GLuint make_shaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader);
