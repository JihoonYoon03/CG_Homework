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
	glm::vec3 vertices[24] =
	{
		// 앞면 (0-3): Z = +0.5
		{ -0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f,  0.5f },
		{  0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f },

		// 왼쪽면 (4-7): X = -0.5
		{ -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f,  0.5f },
		{ -0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f, -0.5f },

		// 윗면 (8-11): Y = +0.5
		{ -0.5f,  0.5f,  0.5f }, {  0.5f,  0.5f,  0.5f },
		{  0.5f,  0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f },

		// 오른쪽면 (12-15): X = +0.5
		{  0.5f, -0.5f,  0.5f }, {  0.5f, -0.5f, -0.5f },
		{  0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f,  0.5f },

		// 아랫면 (16-19): Y = -0.5
		{ -0.5f, -0.5f, -0.5f }, {  0.5f, -0.5f, -0.5f },
		{  0.5f, -0.5f,  0.5f }, { -0.5f, -0.5f,  0.5f },

		// 뒷면 (20-23): Z = -0.5
		{  0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f },
		{ -0.5f,  0.5f, -0.5f }, {  0.5f,  0.5f, -0.5f }
	};

	glm::vec3 normals[24] =
	{
		// 앞면
		{  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },
		{  0.0f,  0.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },

		// 왼쪽면
		{ -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f },
		{ -1.0f,  0.0f,  0.0f }, { -1.0f,  0.0f,  0.0f },

		// 윗면
		{  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f },
		{  0.0f,  1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f },

		// 오른쪽면
		{  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f },
		{  1.0f,  0.0f,  0.0f }, {  1.0f,  0.0f,  0.0f },

		// 아랫면
		{  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f },
		{  0.0f, -1.0f,  0.0f }, {  0.0f, -1.0f,  0.0f },

		// 뒷면
		{  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f },
		{  0.0f,  0.0f, -1.0f }, {  0.0f,  0.0f, -1.0f }
	};

	glm::vec3 colors[24] =
	{
		// 앞면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 왼쪽면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 윗면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 오른쪽면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 아랫면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },

		// 뒷면
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }
	};

	GLuint indices[36] =
	{
		// 앞면
		0, 1, 2,    0, 2, 3,
		// 왼쪽면
		4, 5, 6,    4, 6, 7,
		// 윗면
		8, 9, 10,   8, 10, 11,
		// 오른쪽면
		12, 13, 14, 12, 14, 15,
		// 아랫면
		16, 17, 18, 16, 18, 19,
		// 뒷면
		20, 21, 22, 20, 22, 23
	};

	GLuint VAO, VERTEX, FACE, COLOR, NORMAL;

public:
	Cube(glm::vec3 scaling = { 1.0f, 1.0f, 1.0f }, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec3 location = { 0.0f, 0.0f, 0.0f }, glm::vec3 color = { 1.0f, 1.0f, 1.0f });
	void Render();
};

class Maze {
	int row, col;
	GLfloat width = 2.0f, length = 2.0f;
	std::vector<Cube> walls;
public:
	Maze(int row, int col);
	void Render();
};

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
