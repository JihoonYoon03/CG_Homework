#include <iostream>
#include <stdlib.h>
#include <stdio.h>	
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include "homework_tools.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#pragma warning(disable: 4711 4710 4100)

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunc(int value);

GLint winWidth = 1280, winHeight = 720;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

glm::vec3 bgColor = { 0.02f, 0.02f, 0.02f };

Maze* maze;
DisplayBasis* XYZ;

glm::vec3 EYE{ 0.0f, 3.0f, 5.0f };
glm::vec3 AT{ 0.0f, 1.0f, 0.0f };
glm::vec3 UP{ 0.0f, 1.0f, 0.0f };
GLfloat camera_speed = 0.1f, camera_y_angle = 0.0f, camera_mov_dir = 0.0f, camera_rot_dir = 0.0f;

glm::vec3 lightPos{ 3.0f, 3.0f, 3.0f };
glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
glm::vec3 viewPos = EYE;
float shininess = 32.0f;

bool perspectiveOn = true;

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Example1");

	srand((unsigned int)time(0));

	glewExperimental = GL_TRUE;
	glewInit();

	make_vertexShaders(vertexShader, "vertex.glsl");
	make_fragmentShaders(fragmentShader, "fragment.glsl");
	shaderProgramID = make_shaderProgram(vertexShader, fragmentShader);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// 데이터 초기화
	XYZ = new DisplayBasis(1.0f);

	bool generating = true;
	while (generating) {
		int row, col;
		std::cout << "가로와 세로로 나눌 횟수를 입력해주세요.\n나누기 제한 : 5 ~ 25\n입력: ";
		std::cin >> col >> row;
		if (std::cin.fail() || row < 5 || row > 25 || col < 5 || col > 25) {
			std::cout << "잘못된 입력입니다.\n";
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}
		maze = new Maze(row, col);
		generating = false;
	}

	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1000 / 60, TimerFunc, 1);
	glutMainLoop();
	delete maze;
}

GLvoid drawScene()
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform1f(glGetUniformLocation(shaderProgramID, "shininess"), shininess);
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), viewPos.x, viewPos.y, viewPos.z);

	glm::mat4 projection;
	if (perspectiveOn) projection = glm::perspective(glm::radians(55.0f), static_cast<GLfloat>(winWidth) / winHeight, 0.1f, 100.0f);
	else projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);

	glm::mat4 view = glm::lookAt(EYE, AT, UP);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glUniform1f(glGetUniformLocation(shaderProgramID, "roofYOffset"), 0.0f);
	XYZ->Render();
	maze->Render(shaderProgramID);
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	winWidth = w;
	winHeight = h;
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'o':
		perspectiveOn = false;
		break;
	case 'p':
		perspectiveOn = true;
		break;
	case 'z': case 'Z':
		if (perspectiveOn) {
			camera_mov_dir = key == 'z' ? -1.0f : 1.0f;
			EYE += camera_speed * glm::vec3(0.0f, 0.0f, camera_mov_dir);
			AT += camera_speed * glm::vec3(0.0f, 0.0f, camera_mov_dir);
		}
		break;
	case 'm':
		maze->setRoofMoving(true);
		break;
	case 'M':
		maze->setRoofMoving(false);
		break;
	case 'y': case 'Y':
		if (camera_rot_dir == 0.0f)
			camera_rot_dir = key == 'y' ? -1.0f : 1.0f;
		else
			camera_rot_dir = 0.0f;
		break;
	case 'r':
		break;
	case 'v':
		maze->setRoofHeight(-0.4f);
		maze->displayPlayer(true);
		break;
	case 'q':
		exit(0);
		break;
	}
}

GLvoid TimerFunc(int value)
{
	if (maze->animating()) {
		maze->startingAnimation();
	}
	else if (maze->roofMoving()) {
		maze->roofAnimation();
	}
	if (camera_rot_dir) {
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(camera_rot_dir), glm::vec3(0.0f, 1.0f, 0.0f));
		EYE = glm::vec3(rotation * glm::vec4(EYE, 1.0f));
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, TimerFunc, 1);
}