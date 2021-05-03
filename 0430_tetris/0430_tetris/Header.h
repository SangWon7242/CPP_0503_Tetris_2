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

int displayData[GRID_HEIGHT][GRID_WIDTH] = { 0, }; // �迭�� (x, y)�� �ݴ��̴�. grid_height(��), grid_width(��)


class Display { // �ֿܼ� �ð����� ȿ���� �ο�
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

class GameEngine { // �ΰ��� �����͸� ����
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
	}; // �׸��� �����͸� ����
	int userBlock[USERBLOCK_SIZE][USERBLOCK_SIZE] = { 0, }; // ��� ������

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
	int blockY = 0; // ��������� ��ǥ�� ����� ����

	float elapsed = 0.0f;
	float controlCheck = 0.0f;

	void init() {
		// ���ʿ� ���ӿ����� �ʱ�ȭ �ϴ� ����
		makeUserBlock();

	}

	void next(float dt, char keyboardInput) {
		// Ű���� �Է°��� �޾ƿͼ� � ���� ���� ����(while �������� �Ź� �ҷ�����)	
		/*blockY++;*/

		if (state == GameState::GAMEOVER) return; //�ƹ��͵� ���� x

		elapsed = elapsed + dt; // elapsed += dt;
		if (elapsed >= 0.5f) {
			if (canGoDown()) {
				blockY++;
			}
			else {
				// �� ������ �� ������ userBlock�� gameGridData�� ����
				trans();
				if (gameOverDecision()) state = GameState::GAMEOVER; // ���ӿ����� ���¼� next�� ����Ǽ��� �ȵ�
			}
			elapsed = elapsed - 0.5f; // elapsed -= dt;
		}

		controlCheck = controlCheck + dt; // �ð��� ���� ������

		if (keyboardInput == 'a' && canGoLeft() && controlCheck > 0.1) { // controlCheck�� 0.1�ʸ��� ����������
			blockX--; // �������� ��ĭ��
			controlCheck = 0.0f;
		}
		else if (keyboardInput == 'd' && canGoRight() && controlCheck > 0.1) {
			blockX++; // �������� ��ĭ��
			controlCheck = 0.0f;
		}
		else if (keyboardInput == 's' && canGoDown() && controlCheck > 0.1) {
			blockY++; // �������� ��ĭ��
			controlCheck = 0.0f;
		}

	}

	//����� �Ʒ��� ������ �� �ֳ�
	bool canGoDown() {
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				if (userBlock[i][k] == 1 && i + blockY + 1 >= GRID_HEIGHT) // �迭 ������ ���� ������ ���
					return false;
				if (userBlock[i][k] == 1 && gameGridData[i + blockY + 1][k + blockX] == 1)
					return false;
			}
		}
		return true;
	}

	//����� �������� �� �� �ֳ�

	// i, k�� ���鼭 
	bool canGoLeft() {
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				if (userBlock[i][k] == 1 && k + blockX - 1 < 0) { // ����� �׸��� ���� ���������� ��� -1�� ���
					return false;
				}
				if (userBlock[i][k] == 1 && gameGridData[i + blockY][k + blockX - 1] == 1) { // �������� �̵����� ���ϴ� ���
					return false;
				}
			}
		}
		return true;
	}
	//����� ���������� �� �� �ֳ�
	bool canGoRight() {
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				if (userBlock[i][k] == 1 && k + blockX + 1 > GRID_WIDTH - 1) { // ����� �׸��� ���� ���������� ��� -1�� ���
					return false;
				}
				if (userBlock[i][k] == 1 && gameGridData[i + blockY][k + blockX + 1]) { // �������� �̵����� ���ϴ� ���
					return false;
				}
			}
		}
		return true;
	}

	// ���� �� �ִ��� Ȯ��
	bool isLineFilled(int y) {
		for (int i = 0; i < GRID_WIDTH; i++) {
			if (gameGridData[y][i] == 0) return false;
		}
		return true;
	}

	void eraseLine(int y) {
		for (int i = 0; i < GRID_WIDTH; i++) {
			gameGridData[y][i] = 0; // y��° ���� 0���� ����
		}
	}

	void drop(int y) { // y��ǥ�� �������� �� ĭ�� �Ʒ��� ����
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
		// �� ���� ���� �� �ִ��� Ȯ��
		for (int i = 0; i < GRID_HEIGHT; i++) {
			if (isLineFilled(i)) {
				eraseLine(i);
				drop(i);
			}
		}

		// ���ο� ��� ����
		makeUserBlock();
	}

	bool gameOverDecision() { // ���� �� �� ��� ���α׷� ����.
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

		//������ ���ؼ� ���ο� ����� �����.
		//srand(time(0));

		int various = rand() % 3;
		for (int i = 0; i < USERBLOCK_SIZE; i++) {
			for (int k = 0; k < USERBLOCK_SIZE; k++) {
				userBlock[i][k] = userBlcokVarious[various][i][k];
			}
		}
	}

	void rotate() {
		// TODO :: ȸ�������ϱ�
	}

	void makeDisplayData() { // ���� ���� �����͸� ȭ�鿡 ����� �� �ִ� �����ͷ� �ٲ��ش�.
		for (int i = 0; i < GRID_HEIGHT; i++) {
			for (int k = 0; k < GRID_WIDTH; k++) {
				displayData[i][k] = gameGridData[i][k];
			}
		}

		for (int i = 0; i < USERBLOCK_SIZE; i++) { // ����� �׸��� ������ �������� ��� �ƹ��͵� �ϸ� �ȵȴ�.
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
// ���� �ڵ�� ���� �ڵ�
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
