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
	transform_queue.push(glm::scale(glm::mat4(1.0f), amount));
}

void Cube::rotating(glm::vec3 amount) {
	glm::mat4 rotate = glm::mat4(1.0f);
	rotate = glm::rotate(rotate, glm::radians(amount.x), glm::vec3(1.0f, 0.0f, 0.0f));
	rotate = glm::rotate(rotate, glm::radians(amount.y), glm::vec3(0.0f, 1.0f, 0.0f));
	rotate = glm::rotate(rotate, glm::radians(amount.z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform_queue.push(rotate);
}

void Cube::translating(glm::vec3 amount) {
	transform_queue.push(glm::translate(glm::mat4(1.0f), amount));
}

glm::mat4 Cube::getModelMatrix() {
	while (!transform_queue.empty()) {
		additional_transform *= transform_queue.front();
		transform_queue.pop();
	}

	// 추가 변환 * 초기 변환
	return additional_transform * initial_translate * initial_rotate * initial_scale;
}

glm::vec3 Cube::getCenter() {
	glm::mat4 model = getModelMatrix();  // 큐 처리
	return glm::vec3(model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
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
	roof_move_amount = height - initial_scale[1].y * 1.0f;
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

glm::vec4 Cube::getBoundingBox() {
	glm::mat4 modelMatrix = getModelMatrix();
	glm::vec3 minX = { -0.5f, 0.0f, 0.0f }, maxX = { 0.5f, 0.0f, 0.0f };
	glm::vec3 minZ = { 0.0f, 0.0f, -0.5f }, maxZ = { 0.0f, 0.0f, 0.5f };
	minX = glm::vec3(modelMatrix * glm::vec4(minX, 1.0f));
	maxX = glm::vec3(modelMatrix * glm::vec4(maxX, 1.0f));
	minZ = glm::vec3(modelMatrix * glm::vec4(minZ, 1.0f));
	maxZ = glm::vec3(modelMatrix * glm::vec4(maxZ, 1.0f));
	return glm::vec4(minX.x, maxX.x, minZ.z, maxZ.z);
}

void Cube::Render() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Cube::reset() {
	while (!transform_queue.empty())
		transform_queue.pop();
	additional_transform = glm::mat4(1.0f);
	roof_move_speed_offset = 0.0f;
	roof_move_amount = 0.0f;
}

Player::Player(glm::vec3 scaling, glm::vec3 rotation, glm::vec3 location, glm::vec3 color) : Cube(scaling, rotation, location, color) {}

void Player::move(std::vector<Cube>& walls, const std::vector<bool>& isWall, const glm::vec4& groundBounds) {
	if (move_delta_x == 0.0f && move_delta_z == 0.0f) return;

	// 원본 delta 백업
	GLfloat original_delta_x = move_delta_x;
	GLfloat original_delta_z = move_delta_z;
	
	// 이동량을 임시 변수로 관리
	GLfloat temp_delta_x = move_delta_x;
	GLfloat temp_delta_z = move_delta_z;

	// 먼저 이동을 적용하여 최신 위치 반영
	this->translating({ temp_delta_x, 0.0f, temp_delta_z });
	
	// 최신 바운딩 박스로 충돌 체크
	glm::vec4 playerBox = getBoundingBox();

	// groundBounds: x = minX, y = maxX, z = minZ, w = maxZ
	// 바닥 경계를 벗어나는지 체크
	bool xBlocked = false;
	bool zBlocked = false;
	
	const float epsilon = 0.0001f;
	
	// 바닥 경계 체크
	if (playerBox.x < groundBounds.x + epsilon) {
		xBlocked = true;
	}
	else if (playerBox.y > groundBounds.y - epsilon) {
		xBlocked = true;
	}
	
	if (playerBox.z < groundBounds.z + epsilon) {
		zBlocked = true;
	}
	else if (playerBox.w > groundBounds.w - epsilon) {
		zBlocked = true;
	}

	// 벽과 충돌 체크
	if (!xBlocked || !zBlocked) {
		for (size_t i = 0; i < walls.size(); i++) {
			if (!isWall[i]) continue;

			glm::vec4 wallBox = walls[i].getBoundingBox();

			// 현재 겹침 상태 확인
			bool xOverlap = (playerBox.y > wallBox.x) && (playerBox.x < wallBox.y);
			bool zOverlap = (playerBox.w > wallBox.z) && (playerBox.z < wallBox.w);

			// 완전히 겹쳐있으면 충돌
			if (xOverlap && zOverlap) {
				// X축 방향 체크
				if (original_delta_x > epsilon) {
					// 오른쪽에서 왼쪽 벽에 충돌
					xBlocked = true;
					std::cout << "Blocked X+ direction at wall " << i << "\n";
				}
				else if (original_delta_x < -epsilon) {
					// 왼쪽에서 오른쪽 벽에 충돌
					xBlocked = true;
					std::cout << "Blocked X- direction at wall " << i << "\n";
				}

				if (original_delta_z > epsilon) {
					zBlocked = true;
					std::cout << "Blocked Z+ direction at wall " << i << "\n";
				}
				else if (original_delta_z < -epsilon) {
					zBlocked = true;
					std::cout << "Blocked Z- direction at wall " << i << "\n";
				}
			}
		}
	}

	// 충돌한 경우 이동 취소 후 방향별로 재이동
	if (xBlocked || zBlocked) {
		// 방금 적용한 이동 취소
		this->translating({ -temp_delta_x, 0.0f, -temp_delta_z });
		
		// 막히지 않은 축만 이동
		GLfloat safe_x = xBlocked ? 0.0f : temp_delta_x;
		GLfloat safe_z = zBlocked ? 0.0f : temp_delta_z;
		
		if (safe_x != 0.0f || safe_z != 0.0f) {
			this->translating({ safe_x, 0.0f, safe_z });
		}
	}

	// 원본 delta 복원 (키 입력 상태 유지)
	move_delta_x = original_delta_x;
	move_delta_z = original_delta_z;
}

glm::vec3 Player::getEyeFPS() {
	glm::vec4 eyePos = getModelMatrix() * glm::vec4(0.0f, 0.55f, 0.3f, 1.0f);
	return glm::vec3(eyePos);
}

glm::vec3 Player::getAtFPS() {
	glm::vec4 atPos = getModelMatrix() * glm::vec4(0.0f, 0.55f, -0.7f, 1.0f);
	return glm::vec3(atPos);
}

glm::vec3 Player::getEyeTPS() {
	glm::vec4 eyePos = getModelMatrix() * glm::vec4(0.0f, 1.5f, 7.0f, 1.0f);
	return glm::vec3(eyePos);
}

glm::vec3 Player::getAtTPS() {
	glm::vec4 atPos = getModelMatrix() * glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
	return glm::vec3(atPos);
}

void Player::reset() {
	Cube::reset();
	move_delta_x = 0.0f;
	move_delta_z = 0.0f;
}

Maze::Maze(int row, int col) : row(row), col(col) {
	GLfloat widthPerCol = width / col;
	GLfloat lengthPerRow = length / row;

	ground = new Cube(glm::vec3(width, 0.00005f, length), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	srand(static_cast<unsigned int>(time(0)));
	GLint start_row = row - 1, start_col, end_row = 0, end_col;
	start_col = 1 + 2 * (rand() % ((col - 2) / 2));
	end_col = 1 + 2 * (rand() % ((col - 2) / 2));

	for (int y = 0; y < row; y++) {
		for (int x = 0; x < col; x++) {
			glm::vec3 xyz{
				-width / 2.0f + widthPerCol / 2 + widthPerCol * x,
				0.5f,
				-length / 2.0f + lengthPerRow / 2 + lengthPerRow * y
			};

			bool start_pos = false;
			if (y == start_row && x == start_col) {
				GLfloat scaleX = widthPerCol * 0.2f;
				GLfloat scaleZ = lengthPerRow * 0.2f;
				player_start_pos = glm::vec3(xyz.x, scaleX + scaleZ, xyz.z);
				player_start_idx = { x, y };
				player = new Player(glm::vec3(scaleX, scaleX + scaleZ, scaleZ), glm::vec3(0.0f, 0.0f, 0.0f), player_start_pos, glm::vec3(1.0f, 0.0f, 0.0f));
				start_pos = true;
			}

			bool end_pos = false;
			if (y == end_row && x == end_col) {
				maze_end_pos = xyz;
				maze_end_idx = { x, y };
				end_pos = true;
			}

			glm::vec3 color{
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f,
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f,
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f
			};

			animation_goal.push_back(xyz);

			glm::vec3 offset = color * 4.0f;
			animation_start.push_back(xyz + offset);

			walls.push_back(Cube(glm::vec3(widthPerCol, 1.0f, lengthPerRow), glm::vec3(0.0f, 0.0f, 0.0f), xyz, color));
			walls.back().translating(offset);

			// 행 또는 열이 짝수개일 때, 최외곽 -1 번째 벽은 반드시 통로로
			if ((col % 2 == 0 && x == col - 2 && y > 0 && y < row - 1) ||
				(row % 2 == 0 && y == row - 2 && x > 0 && x < col - 1)) {
				
				// 복잡성을 위해 홀수 통로는 뚫고
				if (x % 2 == 1 || y % 2 == 1)
					isWall.push_back(false);

				// 짝수 벽은 랜덤
				else
					isWall.push_back(rand() % 2 == 0 ? false : true);
			}
			else if ((x % 2 == 0 || y % 2 == 0 || x == col - 1 || y == row - 1) && !start_pos && !end_pos) {
				isWall.push_back(true);
			}
			else {
				isWall.push_back(false);
				if (start_pos || end_pos) continue;

				// 우측 통로에 대한 엣지 비용 설정 및 edges에 양방향 추가
				if (x + 2 < col) {
					int randVal = rand() % 100;
					IndexPos u{ x, y };
					IndexPos v{ x + 2, y };
					edges[u].push_back({ randVal, v });
					edges[v].push_back({ randVal, u });
				}

				// 아래 통로에 대한 ''
				if (y + 2 < row) {
					int randVal = rand() % 100;
					IndexPos u{ x, y };
					IndexPos v{ x, y + 2 };
					edges[u].push_back({ randVal, v });
					edges[v].push_back({ randVal, u });
				}
			}
		}
	}
}

void Maze::startingAnimation() {
	for (size_t i = 0; i < walls.size(); i++) {
		glm::vec3 current_pos = walls[i].getCenter();

		glm::vec3 distance = animation_goal[i] - animation_start[i];
		glm::vec3 target_pos = animation_start[i] + distance * animation_elapsed;

		if (animation_elapsed >= 1.0f) {
			isAnimating = false;
			target_pos = animation_goal[i];
		}

		glm::vec3 delta = target_pos - current_pos;

		walls[i].translating(delta);
	}
	animation_elapsed += animation_speed;
}

void Maze::roofAnimation() {
	for (auto& wall : walls) {
		wall.roofMove();
	}
}

void Maze::lockRoofHeight(GLfloat height) {
	roof_moving = false;
	for (auto& wall : walls) {
		wall.setRoofHeight(height);
	}
}

void Maze::unlockRoofHeight() {
	roof_moving = true;
	for (auto& wall : walls) {
		wall.setRoofHeight(1.0f);
	}
}

void Maze::addRoofMoveSpeed(GLfloat speed) {
	for (auto& wall : walls) {
		wall.addRoofMoveSpeed(speed);
	}
}

void Maze::findPath() {
	std::map<IndexPos, bool> added;
	std::map<IndexPos, IndexPos> parent;
	std::map<IndexPos, int> best;

	for (int y = 1; y < row - 1; y += 2)
	{
		for (int x = 1; x < col - 1; x += 2)
		{
			IndexPos pos{ x, y };
			best[pos] = INT32_MAX;
			added[pos] = false;
		}
	}

	std::priority_queue<CostEdge> pq;
	IndexPos startPos = { col / 2, row / 2 };
	if (row / 2 % 2 == 0) startPos.row += 1;
	if (col / 2 % 2 == 0) startPos.col += 1;

	pq.push(CostEdge{ 0, startPos });
	best[startPos] = 0;
	parent[startPos] = startPos;

	while (!pq.empty())
	{
		CostEdge bestEdge = pq.top();
		pq.pop();

		IndexPos v = bestEdge.pos;

		// 이미 추가되었다면 스킵
		if (added[v])
			continue;

		added[v] = true;

		// 시작점이 아닌 경우에만 벽 제거
		if (!(parent[v] == v)) {
			int wall_x = (parent[v].col + v.col) / 2;
			int wall_y = (parent[v].row + v.row) / 2;
			int wall_idx = wall_y * col + wall_x;
			isWall[wall_idx] = false;
		}

		// 인접한 간선들을 검사
		auto it = edges.find(v);
		if (it != edges.end()) {
			for (const CostEdge& edge : it->second)
			{
				if (added[edge.pos])
					continue;

				if (edge.cost >= best[edge.pos])
					continue;

				best[edge.pos] = edge.cost;
				parent[edge.pos] = v;
				pq.push({ -edge.cost, edge.pos });
			}
		}
	}
}

void Maze::makeMaze() {
	roof_moving = false;
	findPath();
	for (int i = 0; i < row * col; i++) {
		glm::vec3 wall_pos = walls[i].getCenter();
		if (isWall[i] && wall_pos != player_start_pos && wall_pos != maze_end_pos) {
			walls[i].setRoofHeight(0.2f);
		}
		else {
			walls[i].setRoofHeight(0.0f);
		}
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

void Maze::reset(int newrow, int newcol) {
	delete player;
	player = nullptr;

	for (auto& wall : walls) {
		wall.reset();
	}

	animation_elapsed = 0.0f;
	isAnimating = true;
	roof_moving = false;
	display_player = false;
	animation_elapsed = 0.0f;

	walls.clear();
	isWall.clear();
	edges.clear();

	animation_start.clear();
	animation_goal.clear();
	
	row = newrow;
	col = newcol;

	// 재생성
	GLfloat widthPerCol = width / col;
	GLfloat lengthPerRow = length / row;

	GLint start_row = row - 1, start_col, end_row = 0, end_col;
	start_col = 1 + 2 * (rand() % ((col - 2) / 2));
	end_col = 1 + 2 * (rand() % ((col - 2) / 2));

	for (int y = 0; y < row; y++) {
		for (int x = 0; x < col; x++) {
			glm::vec3 xyz{
				-width / 2.0f + widthPerCol / 2 + widthPerCol * x,
				0.5f,
				-length / 2.0f + lengthPerRow / 2 + lengthPerRow * y
			};

			bool start_pos = false;
			if (y == start_row && x == start_col) {
				GLfloat scaleX = widthPerCol * 0.2f;
				GLfloat scaleZ = lengthPerRow * 0.2f;
				player_start_pos = glm::vec3(xyz.x, scaleX + scaleZ, xyz.z);
				player_start_idx = { x, y };
				player = new Player(glm::vec3(scaleX, scaleX + scaleZ, scaleZ), glm::vec3(0.0f, 0.0f, 0.0f), player_start_pos, glm::vec3(1.0f, 0.0f, 0.0f));
				start_pos = true;
			}

			bool end_pos = false;
			if (y == end_row && x == end_col) {
				maze_end_pos = xyz;
				maze_end_idx = { x, y };
				end_pos = true;
			}

			glm::vec3 color{
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f,
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f,
				static_cast<GLfloat>(rand()) / RAND_MAX * 1.5f - 0.5f
			};

			animation_goal.push_back(xyz);

			glm::vec3 offset = color * 4.0f;
			animation_start.push_back(xyz + offset);

			walls.push_back(Cube(glm::vec3(widthPerCol, 1.0f, lengthPerRow), glm::vec3(0.0f, 0.0f, 0.0f), xyz, color));
			walls.back().translating(offset);

			// 행 또는 열이 짝수개일 때, 최외곽 -1 번째 벽은 반드시 통로로
			if ((col % 2 == 0 && x == col - 2 && y > 0 && y < row - 1) ||
				(row % 2 == 0 && y == row - 2 && x > 0 && x < col - 1)) {

				// 복잡성을 위해 홀수 통로는 뚫고
				if (x % 2 == 1 || y % 2 == 1)
					isWall.push_back(false);

				// 짝수 벽은 랜덤
				else
					isWall.push_back(rand() % 2 == 0 ? false : true);
			}
			else if ((x % 2 == 0 || y % 2 == 0 || x == col - 1 || y == row - 1) && !start_pos && !end_pos) {
				isWall.push_back(true);
			}
			else {
				isWall.push_back(false);
				if (start_pos || end_pos) continue;

				// 우측 통로에 대한 엣지 비용 설정 및 edges에 양방향 추가
				if (x + 2 < col) {
					int randVal = rand() % 100;
					IndexPos u{ x, y };
					IndexPos v{ x + 2, y };
					edges[u].push_back({ randVal, v });
					edges[v].push_back({ randVal, u });
				}

				// 아래 통로에 대한 ''
				if (y + 2 < row) {
					int randVal = rand() % 100;
					IndexPos u{ x, y };
					IndexPos v{ x, y + 2 };
					edges[u].push_back({ randVal, v });
					edges[v].push_back({ randVal, u });
				}
			}
		}
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