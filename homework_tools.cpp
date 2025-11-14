#include "homework_tools.h"

Cube::Cube(glm::vec3 scaling, glm::vec3 rotation, glm::vec3 location, glm::vec3 color) : center(location) {
	// 초기 변환 행렬 설정
	initial_translate = glm::translate(glm::mat4(1.0f), location);
	
	initial_rotate = glm::mat4(1.0f);
	initial_rotate = glm::rotate(initial_rotate, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	initial_rotate = glm::rotate(initial_rotate, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	initial_rotate = glm::rotate(initial_rotate, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	
	initial_scale = glm::scale(glm::mat4(1.0f), scaling);
	
	// 원본 정점 데이터를 vertices 배열에 복사
	for (int i = 0; i < 24; i++) {
		vertices[i] = original_vertices[i];
		colors[i] = color;
	}

	roof_move_cap_upper = static_cast<GLfloat>(rand()) / RAND_MAX * 1.4f + 1.0f;
	roof_move_cap_lower = static_cast<GLfloat>(rand()) / RAND_MAX * 0.5f * scaling.y - 1.0f * scaling.y;
	roof_move_speed = static_cast<GLfloat>(rand()) / RAND_MAX * 0.05f + 0.005f;
	roof_move_dir = 1.0f;
	roof_move_amount = 0.0f;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VERTEX);
	glGenBuffers(1, &FACE);
	glGenBuffers(1, &COLOR);
	glGenBuffers(1, &NORMAL);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VERTEX);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FACE);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, COLOR);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, NORMAL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
	glEnableVertexAttribArray(2);
}

void Cube::scaling(glm::vec3 amount) {
	scale = glm::scale(scale, amount);
}

void Cube::rotating(glm::vec3 amount) {
	rotate = glm::rotate(rotate, glm::radians(amount.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotate = glm::rotate(rotate, glm::radians(amount.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotate = glm::rotate(rotate, glm::radians(amount.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Cube::translating(glm::vec3 amount) {
	translate = glm::translate(translate, amount);
}

glm::mat4 Cube::getModelMatrix() const {
	// 추가 변환 * 초기 변환
	return translate * rotate * scale * initial_translate * initial_rotate * initial_scale;
}

void Cube::roofMove() {
	GLfloat temp = roof_move_amount + (roof_move_speed + roof_move_speed_offset) * roof_move_dir;
	if (temp >= roof_move_cap_upper || temp <= roof_move_cap_lower) {
		temp = temp >= roof_move_cap_upper ? roof_move_cap_upper : roof_move_cap_lower;
		roof_move_dir = -roof_move_dir;
	}
	roof_move_amount = temp;
}

void Cube::setRoofHeight(GLfloat height) {
	roof_move_amount = height;
}

void Cube::addRoofMoveSpeed(GLfloat speed) {
	roof_move_speed_offset += speed;
	if (roof_move_speed + roof_move_speed_offset < 0.001f) {
		roof_move_speed_offset = 0.001f - roof_move_speed;
	}

	if (roof_move_speed + roof_move_speed_offset > 0.5f) {
		roof_move_speed_offset = 0.5f - roof_move_speed;
	}
}

void Cube::Render() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Cube::reset() {
	translate = glm::mat4(1.0f);
	rotate = glm::mat4(1.0f);
	scale = glm::mat4(1.0f);
	roof_move_speed_offset = 0.0f;
	roof_move_amount = 0.0f;
}

Player::Player(glm::vec3 scaling, glm::vec3 rotation, glm::vec3 location, glm::vec3 color) : Cube(scaling, rotation, location, color) {}

void Player::move() {
	if (move_delta_x == 0.0f && move_delta_z == 0.0f) return;

	move_amount_x += move_delta_x;
	move_amount_z += move_delta_z;
	
	// 추가 변환을 초기화하고 새로운 위치로 설정
	translate = glm::translate(glm::mat4(1.0f), glm::vec3(move_amount_x, 0.0f, move_amount_z));
}

glm::vec3 Player::getEyeFPS() const {
	glm::vec4 eyePos = getModelMatrix() * glm::vec4(0.0f, 0.55f, 0.0f, 1.0f);
	return glm::vec3(eyePos);
}

glm::vec3 Player::getAtFPS() const {
	glm::vec4 atPos = getModelMatrix() * glm::vec4(0.0f, 0.55f, -1.0f, 1.0f);
	return glm::vec3(atPos);
}

glm::vec3 Player::getEyeTPS() const {
	glm::vec4 eyePos = getModelMatrix() * glm::vec4(0.0f, 1.5f, 7.0f, 1.0f);
	return glm::vec3(eyePos);
}

glm::vec3 Player::getAtTPS() const {
	glm::vec4 atPos = getModelMatrix() * glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
	return glm::vec3(atPos);
}

void Player::reset() {
	Cube::reset();
	move_amount_x = 0.0f;
	move_amount_z = 0.0f;
	move_delta_x = 0.0f;
	move_delta_z = 0.0f;
}

Maze::Maze(int row, int col) : row(row), col(col) {
	GLfloat widthPerCol = width / col;
	GLfloat lengthPerRow = length / row;

	ground = new Cube(glm::vec3(width, 0.00005f, length), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f));

	srand(static_cast<unsigned int>(time(0)));
	GLint start_row = row - 1, start_col, end_row = 0, end_col;
	start_col = rand() % (col - 2) + 1;
	end_col = rand() % (col - 2) + 1;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			glm::vec3 xyz{
				-width / 2.0f + widthPerCol / 2 + widthPerCol * j,
				0.5f,
				-length / 2.0f + lengthPerRow / 2 + lengthPerRow * i
			};

			if (i == start_row && j == start_col) {
				GLfloat scaleX = widthPerCol * 0.2f;
				GLfloat scaleZ = lengthPerRow * 0.2f;
				player_start_pos = glm::vec3(xyz.x, scaleX + scaleZ, xyz.z);
				player = new Player(glm::vec3(scaleX, scaleX + scaleZ, scaleZ), glm::vec3(0.0f, 0.0f, 0.0f), player_start_pos, glm::vec3(1.0f, 1.0f, 0.0f));
			}
			if (i == end_row && j == end_col) {
				maze_end_pos = xyz;
			}

			glm::vec3 color {
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f,
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f,
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f
			};

			// 메이즈 생성 애니메이션: 목표 위치는 xyz
			animation_goal.push_back(xyz);
			
			// 애니메이션 시작 위치 (목표에서 color 방향으로 4배 떨어진 곳)
			glm::vec3 offset = color * 4.0f;
			animation_start.push_back(xyz + offset);

			// 큐브를 목표 위치(xyz)에 생성하되, 시작 오프셋을 적용
			walls.push_back(Cube(glm::vec3(widthPerCol, 1.0f, lengthPerRow), glm::vec3(0.0f, 0.0f, 0.0f), xyz, color));
			// 초기 오프셋 설정
			walls.back().translate = glm::translate(glm::mat4(1.0f), offset);
		}
	}
}

void Maze::startingAnimation() {
	for (size_t i = 0; i < walls.size(); i++) {
		glm::vec3 start_offset = animation_start[i] - animation_goal[i];
		
		if (animation_elapsed >= 1.0f) {
			isAnimating = false;
			// 애니메이션 완료: 추가 변환 제거 (초기 위치로)
			walls[i].translate = glm::mat4(1.0f);
		}
		else {
			// 선형 보간: start_offset에서 0으로
			glm::vec3 current_offset = start_offset * (1.0f - animation_elapsed);
			walls[i].translate = glm::translate(glm::mat4(1.0f), current_offset);
		}
	}
	animation_elapsed += animation_speed;
}

void Maze::roofAnimation() {
	for (auto& wall : walls) {
		wall.roofMove();
	}
}

void Maze::setRoofHeight(GLfloat height) {
	roof_moving = false;
	for (auto& wall : walls) {
		wall.setRoofHeight(height);
	}
}

void Maze::addRoofMoveSpeed(GLfloat speed) {
	for (auto& wall : walls) {
		wall.addRoofMoveSpeed(speed);
	}
}

void Maze::Render(const GLuint& shaderProgramID) {
	// ground 렌더링
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(ground->getModelMatrix()));
	glUniform1f(glGetUniformLocation(shaderProgramID, "roofYOffset"), 0.0f);
	ground->Render();
	
	// player 렌더링
	if (display_player) {
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(player->getModelMatrix()));
		glUniform1f(glGetUniformLocation(shaderProgramID, "roofYOffset"), 0.0f);
		player->Render();
	}
	
	// walls 렌더링
	for (auto& wall : walls) {
		glm::mat4 model = wall.getModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(shaderProgramID, "roofYOffset"), wall.getRoofMoveAmount());
		wall.Render();
	}
}

void Maze::reset() {
	player->reset();
	for (auto& wall : walls) {
		wall.reset();
	}
	animation_elapsed = 0.0f;
	isAnimating = true;
	roof_moving = false;
	display_player = false;
	animation_elapsed = 0.0f;
}

DisplayBasis::DisplayBasis(GLfloat offset, const glm::vec3& origin) : origin(origin) {

	xyz[0][0] = { origin, { 0.1f, 0.1f, 0.0f } };
	xyz[0][1] = { origin + glm::vec3(1.0f, 0.0f, 0.0f), { 1.0f, 0.0f, 0.0f } };
	xyz[1][0] = { origin, { 0.0f, 0.1f, 0.1f } };
	xyz[1][1] = { origin + glm::vec3(0.0f, 1.0f, 0.0f), { 0.0f, 1.0f, 0.0f } };
	xyz[2][0] = { origin, { 0.1f, 0.0f, 0.1f } };
	xyz[2][1] = { origin + glm::vec3(0.0f, 0.0f, 1.0f), { 0.0f, 0.0f, 1.0f } };

	for (int i = 0; i < 3; i++) {
		xyz[i][0].pos *= offset;
		xyz[i][1].pos *= offset;
	}
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(xyz), xyz, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ColoredVertex), (GLvoid*)sizeof(glm::vec3));
	glEnableVertexAttribArray(1);
}

void DisplayBasis::Render() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 6);
}

std::string read_file(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << filename << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void make_vertexShaders(GLuint& vertexShader, const std::string& shaderName) {
	std::string vertexSource = read_file(shaderName);
	if (vertexSource.empty()) {
		std::cerr << "ERROR: Failed to read vertex shader file" << std::endl;
		return;
	}

	const char* sourcePtr = vertexSource.c_str();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &sourcePtr, nullptr);
	glCompileShader(vertexShader);

	GLint result;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		GLchar errorLog[512];
		glGetShaderInfoLog(vertexShader, 512, nullptr, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
	}
}

void make_fragmentShaders(GLuint& fragmentShader, const std::string& shaderName) {
	std::string fragmentSource = read_file(shaderName);
	if (fragmentSource.empty()) {
		std::cerr << "ERROR: Failed to read fragment shader file" << std::endl;
		return;
	}

	const char* sourcePtr = fragmentSource.c_str();
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &sourcePtr, nullptr);
	glCompileShader(fragmentShader);

	GLint result;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		GLchar errorLog[512];
		glGetShaderInfoLog(fragmentShader, 512, nullptr, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
	}
}

GLuint make_shaderProgram(const GLuint& vertexShader, const GLuint& fragmentShader) {
	GLuint shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	GLint result;
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		GLchar errorLog[512];
		glGetProgramInfoLog(shaderID, 512, nullptr, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return 0;
	}

	glUseProgram(shaderID);
	return shaderID;
}