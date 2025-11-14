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
GLvoid KeySpecial(int key, int x, int y);
GLvoid KeySpecialUp(int key, int x, int y);
GLvoid TimerFunc(int value);

GLint winWidth = 1280, winHeight = 720;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

glm::vec3 bgColor = { 0.02f, 0.02f, 0.02f };

Maze* maze;
DisplayBasis* XYZ;

glm::vec3 EYE_freecam{ 0.0f, 3.0f, 5.0f };
glm::vec3 AT_freecam{ 0.0f, 1.0f, 0.0f };
glm::vec3 UP_freecam{ 0.0f, 1.0f, 0.0f };
GLfloat camera_speed = 0.1f, camera_y_angle = 0.0f, camera_mov_dir = 0.0f, camera_rot_dir = 0.0f;

glm::vec3 EYE_minimap{ 0.0f, 6.0f, 0.0001f };
glm::vec3 AT_minimap{ 0.0f, 0.0f, 0.0f };
glm::vec3 UP_minimap{ 0.0f, 1.0f, 0.0f };

glm::vec3 lightPos{ 3.0f, 3.0f, 3.0f };
glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
glm::vec3 viewPos = EYE_freecam;
float shininess = 32.0f;

bool perspectiveOn = true, cam_far = true, cam_FPS = false, cam_TPS = false;
bool keydown_W = false, keydown_S = false, keydown_A = false, keydown_D = false;

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
	glutSpecialFunc(KeySpecial);
	glutSpecialUpFunc(KeySpecialUp);
	glutTimerFunc(1000 / 60, TimerFunc, 1);
	glutMainLoop();
	delete maze;
}

GLvoid drawScene()
{
	glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);

	// 메인 카메라
	glViewport(0, 0, winWidth, winHeight);
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(shaderProgramID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
	glUniform1f(glGetUniformLocation(shaderProgramID, "shininess"), shininess);
	glUniform3f(glGetUniformLocation(shaderProgramID, "viewPos"), viewPos.x, viewPos.y, viewPos.z);

	glm::mat4 projection;
	if (perspectiveOn) projection = glm::perspective(glm::radians(55.0f), static_cast<GLfloat>(winWidth) / winHeight, 0.1f, 100.0f);
	else projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);

	glm::mat4 view;
	if (cam_far) {
		view = glm::lookAt(EYE_freecam, AT_freecam, UP_freecam);
	}
	if (cam_FPS) {
		view = glm::lookAt(maze->getPlayerEyeFPS(), maze->getPlayerAtFPS(), UP_freecam);
	}
	if (cam_TPS) {
		view = glm::lookAt(maze->getPlayerEyeTPS(), maze->getPlayerAtTPS(), UP_freecam);
	}

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniform1f(glGetUniformLocation(shaderProgramID, "roofYOffset"), 0.0f);
	XYZ->Render();
	maze->Render(shaderProgramID);

	// 미니맵 카메라, 정사각형 범위
	GLint minimap_x = winWidth - winWidth / 5 - 10;
	GLint minimap_y = winHeight - winWidth / 5 - 10;
	glViewport(minimap_x, minimap_y, winWidth / 5, winWidth / 5);

	if (perspectiveOn) projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
	else projection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 100.0f);

	view = glm::lookAt(EYE_minimap, AT_minimap, UP_minimap);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "view"), 1, GL_FALSE, glm::value_ptr(view));

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
			EYE_freecam += camera_speed * glm::vec3(0.0f, 0.0f, camera_mov_dir);
			AT_freecam += camera_speed * glm::vec3(0.0f, 0.0f, camera_mov_dir);
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
		maze->setRoofHeight(-1.0f);
		break;
	case 's':
		// 토글
		maze->togglePlayer();
		if (!maze->isPlayerDisplayed()) {
			cam_FPS = false;
			cam_TPS = false;
			cam_far = true;
		}
		break;
	case '=':
		maze->addRoofMoveSpeed(0.002f);
		break;
	case '-':
		maze->addRoofMoveSpeed(-0.002f);
		break;
	case '1':
		if (maze->isPlayerDisplayed()) {
			if (cam_FPS) {
				cam_FPS = false;
				cam_far = true;
			}
			else {
				cam_FPS = true;
				cam_TPS = false;
				cam_far = false;
			}
		}
		break;
	case '3':
		if (maze->isPlayerDisplayed()) {
			if (cam_TPS) {
				cam_TPS = false;
				cam_far = true;
			}
			else {
				cam_TPS = true;
				cam_FPS = false;
				cam_far = false;
			}
		}
		break;
	case 'c':
		EYE_freecam = { 0.0f, 3.0f, 5.0f };
		AT_freecam = { 0.0f, 1.0f, 0.0f };
		camera_speed = 0.1f, camera_y_angle = 0.0f, camera_mov_dir = 0.0f, camera_rot_dir = 0.0f;

		glm::vec3 viewPos = EYE_freecam;

		perspectiveOn = true, cam_far = true, cam_FPS = false, cam_TPS = false;
		keydown_W = false, keydown_S = false, keydown_A = false, keydown_D = false;
		maze->reset();
		break;
	case 'q':
		exit(0);
		break;
	}
}

GLvoid KeySpecial(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		if (keydown_W == false) {
			keydown_W = true;
			maze->keyUpPressed();
		}
		break;
	case GLUT_KEY_DOWN:
		if (keydown_S == false) {
			keydown_S = true;
			maze->keyDownPressed();
		}
		break;
	case GLUT_KEY_LEFT:
		if (keydown_A == false) {
			keydown_A = true;
			maze->keyLeftPressed();
		}
		break;
	case GLUT_KEY_RIGHT:
		if (keydown_D == false) {
			keydown_D = true;
			maze->keyRightPressed();
		}
		break;
	}
}

GLvoid KeySpecialUp(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
		maze->keyUpReleased();
		keydown_W = false;
		break;
	case GLUT_KEY_DOWN:
		maze->keyDownReleased();
		keydown_S = false;
		break;
	case GLUT_KEY_LEFT:
		maze->keyLeftReleased();
		keydown_A = false;
		break;
	case GLUT_KEY_RIGHT:
		maze->keyRightReleased();
		keydown_D = false;
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
	if (maze->isPlayerDisplayed()) {
		maze->movePlayer();
	}
	if (camera_rot_dir) {
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(camera_rot_dir), glm::vec3(0.0f, 1.0f, 0.0f));
		EYE_freecam = glm::vec3(rotation * glm::vec4(EYE_freecam, 1.0f));
	}
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, TimerFunc, 1);
}