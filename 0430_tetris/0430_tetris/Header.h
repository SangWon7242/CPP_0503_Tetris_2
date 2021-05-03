#define CRT_SECURE_NO_WARNINGS

#include "Tetrishelper.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>

#define GRID_WIDTH 7
#define GRID_HEIGHT 10
#define USERBLOCK_SIZE 3

int displayData[GRID_HEIGHT][GRID_WIDTH] = { 0, }; // 배열은 (x, y)가 반대이다. grid_height(열), grid_width(행)


class Display { // 콘솔에 시각적인 효과를 부여
public:
	void draw() {
		for (int i = 0; i < GRID_HEIGHT; i++) {
			for (int k = 0; k < GRID_WIDTH; k++) {
				if (displayData[i][k] == 0) {
					drawPosition(k, i, false);
				}
				else {
					drawPosition(k, i, true);
				}
			}
		}
	}
};

class GameEngine { // 두가지 데이터를 관리
public:
	enum class GameState {
		PRAYING, GAMEOVER
	};

	GameState state = GameState::PRAYING;

	int gameGridData[GRID_HEIGHT][GRID_WIDTH] = {
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0},
	}; // 그리드 데이터를 관리
	int userBlock[USERBLOCK_SIZE][USERBLOCK_SIZE] = { 0, }; // 블록 사이즈

	int userBlcokVarious[3][USERBLOCK_SIZE][USERBLOCK_SIZE] = {
		{
			{0, 0, 1},
			{0, 0, 1},
			{0, 0, 1}
		},
		{
			{0, 0, 0},
			{0, 1, 1},
			{0, 1, 1}
		},
		{
			{0, 0, 0},
			{0, 1, 0},
			{1, 1, 0}
		},
	};

	int blockX = 0;
	int blockY = 0; // 유저블록의 좌표를 기억할 변수

	float elapsed = 0.0f;
	float controlCheck = 0.0f;

	void init() {
		// 최초에 게임엔진을 초기화 하는 과정
		makeUserBlock();

	}

	void next(float dt, char keyboardInput) {
		// 키보드 입력값을 받아와서 어떤 일을 할지 결정(while 루프에서 매번 불려지는)	
		/*blockY++;*/

		if (state == GameState::GAMEOVER) return; //아무것도 리턴 x

		elapsed = elapsed + dt; // elapsed += dt;
		if (elapsed >= 0.5f) {
			if (canGoDown()) {
				blockY++;
			}
			else {
				// 더 내려갈 수 없으면 userBlock를 gameGridData에 전사
				trans();
				if (gameOverDecision()) state = GameState::GAMEOVER; // 게임오버인 상태서 next는 실행되서는 안됨
			}
			elapsed = elapsed - 0.5f; // elapsed -= dt;
		}

		controlCheck = controlCheck + dt; // 시간이 덮여 씌어짐

		if (keyboardInput == 'a' && canGoLeft() && controlCheck > 0.1) { // controlCheck가 0.1초마다 느려지게함
			blockX--; // 좌측으로 한칸씩
			controlCheck = 0.0f;
		}
		else if (keyboardInput == 'd' && canGoRight() && controlCheck > 0.1) {
			blockX++; // 우측으로 한칸씩
			controlCheck = 0.0f;
		}
		else if (keyboardInput == 's' && canGoDown() && controlCheck > 0.1) {
			blockY++; // 우측으로 한칸씩
			controlCheck = 0.0f;
		}

	}

	//블록이 아래로 내려갈 수 있냐
	bool canGoDown() {
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				if (userBlock[i][k] == 1 && i + blockY + 1 >= GRID_HEIGHT) // 배열 밖으로 나갈 것인지 물어봄
					return false;
				if (userBlock[i][k] == 1 && gameGridData[i + blockY + 1][k + blockX] == 1)
					return false;
			}
		}
		return true;
	}

	//블록이 왼쪽으로 갈 수 있냐

	// i, k를 돌면서 
	bool canGoLeft() {
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				if (userBlock[i][k] == 1 && k + blockX - 1 < 0) { // 블록이 그리드 밖을 빠져나가는 경우 -1인 경우
					return false;
				}
				if (userBlock[i][k] == 1 && gameGridData[i + blockY][k + blockX - 1] == 1) { // 왼쪽으로 이동하지 못하는 경우
					return false;
				}
			}
		}
		return true;
	}
	//블록이 오른쪽으로 갈 수 있냐
	bool canGoRight() {
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				if (userBlock[i][k] == 1 && k + blockX + 1 > GRID_WIDTH - 1) { // 블록이 그리드 밖을 빠져나가는 경우 -1인 경우
					return false;
				}
				if (userBlock[i][k] == 1 && gameGridData[i + blockY][k + blockX + 1]) { // 왼쪽으로 이동하지 못하는 경우
					return false;
				}
			}
		}
		return true;
	}

	// 가득 차 있는지 확인
	bool isLineFilled(int y) {
		for (int i = 0; i < GRID_WIDTH; i++) {
			if (gameGridData[y][i] == 0) return false;
		}
		return true;
	}

	void eraseLine(int y) {
		for (int i = 0; i < GRID_WIDTH; i++) {
			gameGridData[y][i] = 0; // y번째 줄을 0으로 변경
		}
	}

	void drop(int y) { // y좌표를 기준으로 한 칸씩 아래로 내림
		for (int i = y; i >= 0; i--) {
			for (int k = 0; k < GRID_WIDTH; k++) {
				gameGridData[i][k] = i - 1 < 0 ? 0 : gameGridData[i - 1][k];
			}
		}
	}


	void trans() {
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				gameGridData[i + blockY][k + blockX] = userBlock[i][k] == 1 ? userBlock[i][k] : gameGridData[i + blockY][k + blockX];
			}
		}
		// 한 줄이 가득 차 있는지 확인
		for (int i = 0; i < GRID_HEIGHT; i++) {
			if (isLineFilled(i)) {
				eraseLine(i);
				drop(i);
			}
		}

		// 새로운 블록 생성
		makeUserBlock();
	}

	bool gameOverDecision() { // 블럭이 꽉 찬 경우 프로그램 종료.
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				if (userBlock[i][k] == 1 && gameGridData[i + blockY][k + blockX] == 1) {
					return true;
				}
			}
		}
		return false;
	}

	void makeUserBlock() {
		blockX = GRID_WIDTH / 2 - USERBLOCK_SIZE / 2;
		blockY = 0;

		//랜덤을 통해서 새로운 블록을 만든다.
		//srand(time(0));

		int various = rand() % 3;
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				userBlock[i][k] = userBlcokVarious[various][i][k];
			}
		}
	}

	void rotate() {
		// TODO :: 회전구현하기
	}

	void makeDisplayData() { // 실제 게임 데이터를 화면에 출력할 수 있는 데이터로 바꿔준다.
		for (int i = 0; i < GRID_HEIGHT; i++) {
			for (int k = 0; k < GRID_WIDTH; k++) {
				displayData[i][k] = gameGridData[i][k];
			}
		}

		for (int i = 0; i < USERBLOCK_SIZE; i++) { // 블록이 그리드 밖으로 빠져나간 경우 아무것도 하면 안된다.
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				if (i + blockY < 0 || i + blockX > GRID_HEIGHT) {
					//DO NOTHING
				}
				else if (k + blockX < 0 || k + blockX > GRID_WIDTH) {
					//DO NOTHING
				}
				else { //TODO:
					// displayData[i + blockY][k + blockX] = userBlock[i][k] == 1 ? userBlock[i][k] : displayData[i + blockY][k + blockX];
					int _x = k + blockX;
					int _y = i + blockY;

					displayData[_y][_x] = userBlock[i][k] | displayData[_y][_x];
				}
			}
		}
	}
};

/*
// 위의 코드랑 같은 코드
class Display {
public:
	void draw() {
		for (int i = 0; i < GRID_HEIGHT; i++) {
			for (int k = 0; k < GRID_WIDTH; k++) {
				drawPosition(k, i, displayData[i][k] == 1);
			}
		}
	}
};
*/
