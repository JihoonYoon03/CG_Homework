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
GLvoid MouseMotion(int x, int y);
GLvoid TimerFunc(int value);

GLint winWidth = 600, winHeight = 600;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

glm::vec3 bgColor = { 0.1f, 0.1f, 0.1f };
GLfloat m_rotationX = 0.0f, m_rotationY = 0.0f;


Maze* maze;
DisplayBasis* XYZ;

glm::vec3 EYE{ 0.0f, 1.0f, 5.0f };
glm::vec3 AT{ 0.0f, 1.0f, 0.0f };
glm::vec3 UP{ 0.0f, 1.0f, 0.0f };

glm::vec3 lightPos{ 1.0f, 1.0f, 1.0f };
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
	glutPassiveMotionFunc(MouseMotion);
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
	
	glm::mat4 world = glm::rotate(glm::mat4(1.0f), glm::radians(-m_rotationX), glm::vec3(0.0f, 1.0f, 0.0f));
	world = glm::rotate(world, glm::radians(-m_rotationY), glm::vec3(1.0f, 0.0f, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "world"), 1, GL_FALSE, glm::value_ptr(world));

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

GLvoid MouseMotion(int x, int y)
{
	m_rotationX += (x - winWidth / 2) * 0.2f;
	m_rotationY += (y - winHeight / 2) * 0.2f;

	if (m_rotationY > 89.0f) m_rotationY = 89.0f;
	if (m_rotationY < -89.0f) m_rotationY = -89.0f;

	glutWarpPointer(winWidth / 2, winHeight / 2);
	glutPostRedisplay();
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
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, TimerFunc, 1);
}