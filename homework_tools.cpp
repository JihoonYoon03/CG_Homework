#include "homework_tools.h"

Cube::Cube(glm::vec3 scaling, glm::vec3 rotation, glm::vec3 location, glm::vec3 color) : center(location) {
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, location);
	transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, scaling);
	
	for (int i = 0; i < 24; i++) {
		glm::vec4 transformedVertex = transform * glm::vec4(vertices[i], 1.0f);
		vertices[i] = glm::vec3(transformedVertex);
		colors[i] = color;
	}

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
	scale = glm::scale(glm::mat4(1.0f), amount);
}

void Cube::rotating(glm::vec3 amount) {
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(amount.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(amount.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotate = glm::rotate(glm::mat4(1.0f), glm::radians(amount.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Cube::translating(glm::vec3 amount) {
	translate = glm::translate(glm::mat4(1.0f), amount);
}

glm::mat4 Cube::getModelMatrix() {
	return translate * rotate * scale;
}

void Cube::move(glm::vec3 amount) {
	for (int i = 0; i < 24; i++) {
		vertices[i] += amount;
	}
	glBindBuffer(GL_ARRAY_BUFFER, VERTEX);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void Cube::Render() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

Maze::Maze(int row, int col) : row(row), col(col) {
	GLfloat widthPerCol = width / col;
	GLfloat lengthPerRow = length / row;

	srand((unsigned int)time(0));

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			glm::vec3 xyz{
				-width / 2.0f + widthPerCol / 2 + widthPerCol * j,
				0.5f,
				-length / 2.0f + lengthPerRow / 2 + lengthPerRow * i
			};

			glm::vec3 color {
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f,
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f,
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f
			};

			// 메이즈 생성 애니메이션 시작
			animation_start.push_back(xyz + color * 4.0f);

			// 메이즈 생성 애니메이션 목표
			animation_goal.push_back(xyz);

			// 큐브는 랜덤 위치에서 시작
			walls.push_back(Cube(glm::vec3(widthPerCol, 1.0f, lengthPerRow), glm::vec3(0.0f, 0.0f, 0.0f), xyz, color));
			walls.back().translating(color * 4.0f);
		}
	}
}

void Maze::startingAnimation() {
	for (size_t i = 0; i < walls.size(); i++) {
		glm::vec3 distance = animation_goal[i] - animation_start[i];
		glm::vec3 new_location;
		if (animation_elapsed >= 1.0f) {
			isAnimating = false;
			new_location = { 0, 0, 0 };
		}
		else {
			new_location = distance - distance * animation_elapsed;
		}
		walls[i].translating(new_location);
	}
	animation_elapsed += animation_speed;
}

void Maze::Render(const GLuint& shaderProgramID) {
	for (auto& wall : walls) {
		glm::mat4 model = wall.getModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		wall.Render();
	}
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