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
private:
	GLfloat roof_move_amount, roof_move_speed, roof_move_cap_upper, roof_move_cap_lower;

protected:
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

	glm::vec3 center;


	glm::mat4 scale = glm::mat4(1.0f);
	glm::mat4 rotate = glm::mat4(1.0f);
	glm::mat4 translate = glm::mat4(1.0f);

public:
	Cube(glm::vec3 scaling = { 1.0f, 1.0f, 1.0f }, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec3 location = { 0.0f, 0.0f, 0.0f }, glm::vec3 color = { 1.0f, 1.0f, 1.0f });

	void scaling(glm::vec3 amount);
	void rotating(glm::vec3 amount);
	void translating(glm::vec3 amount);
	glm::mat4 getModelMatrix();

	void roofMove();
	void setRoofHeight(GLfloat height);

	void Render();
	glm::vec3 getCenter() const { return center; }
	float getRoofMoveAmount() const { return roof_move_amount; }
};

class Player : public Cube {
	GLfloat move_delta_x = 0.0f, move_delta_z = 0.0f;
	GLfloat move_amount_x = 0.0f, move_amount_z = 0.0f;

public:
	Player(glm::vec3 scaling = { 1.0f, 1.0f, 1.0f }, glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }, glm::vec3 location = { 0.0f, 0.0f, 0.0f }, glm::vec3 color = { 1.0f, 1.0f, 1.0f });

	void addMoveDeltaX(GLfloat delta) { move_delta_x += delta; }
	void addMoveDeltaZ(GLfloat delta) { move_delta_z += delta; }
	void move();
};

class Maze {
	int row, col;
	GLfloat width = 2.0f, length = 2.0f;
	Player* player;
	GLfloat player_speed = 0.002f;
	Cube* ground;
	std::vector<Cube> walls;
	std::vector<glm::vec3> animation_start;
	std::vector<glm::vec3> animation_goal;
	glm::vec3 player_start_pos, maze_end_pos;

	bool isAnimating = true;
	bool roof_moving = false;
	bool display_player = false;

	GLfloat animation_elapsed = 0.0f;
	GLfloat animation_speed = 0.01f;
public:
	Maze(int row, int col);
	void startingAnimation();
	bool animating() const { return isAnimating; }

	void roofAnimation();
	bool roofMoving() const { return roof_moving; }
	void setRoofMoving(bool move) { roof_moving = move; }
	void setRoofHeight(GLfloat height);

	void togglePlayer() { display_player = !display_player; }
	bool isPlayerDisplayed() const { return display_player; }

	void movePlayer() { player->move(); }

	void keyUpPressed() { player->addMoveDeltaZ(-player_speed); }
	void keyDownPressed() { player->addMoveDeltaZ(player_speed); }
	void keyLeftPressed() { player->addMoveDeltaX(-player_speed); }
	void keyRightPressed() { player->addMoveDeltaX(player_speed); }

	void keyUpReleased() { player->addMoveDeltaZ(player_speed); }
	void keyDownReleased() { player->addMoveDeltaZ(-player_speed); }
	void keyLeftReleased() { player->addMoveDeltaX(player_speed); }
	void keyRightReleased() { player->addMoveDeltaX(-player_speed); }

	void Render(const GLuint& shaderProgramID);

	~Maze() {
		delete player;
		delete ground;
	}
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
