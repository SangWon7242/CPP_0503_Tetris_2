#include "Header.h"
#define INTERVAL 1.0/60.0


int main() {
	srand(time(0)); // ���� ����� srand ���� ���� ���´�.

	float prev = (float)clock() / CLOCKS_PER_SEC; // ���α׷� ������ �������� ���ʰ� �������� �� �� �ִ�.
	Display* display = new Display();
	GameEngine* gameEngine = new GameEngine();
	gameEngine->init();
	showConsoleCursor(false); // �����쿡�� Ŀ��Ű�� ������ �ʰ� �ϴ� ��

	while (true) {
		float curr = (float)clock() / CLOCKS_PER_SEC;
		float dt = curr - prev; // �ݺ� �� ������ �߻��ϴ� ���� 

		if (dt < INTERVAL) continue; // �����ִ� 1/60 ���� �۴ٸ� ó������ ���ư���
		prev = curr;

		// printf("%.3f\n", curr);

		bool left = keyState('a');
		bool right = keyState('d');
		bool down = keyState('s');
		bool rotate = keyState('w');

		if (left) {
			// �������� ��� �̵�
			gameEngine->next(dt, 'a');
		}
		else if (right) {
			// ���������� ��� �̵�
			gameEngine->next(dt, 'd');
		}
		else if (down) {
			// �Ʒ������� ��� �̵�
			gameEngine->next(dt, 's');
		}
		else if (rotate) {
			// �Ʒ������� ��� �̵�
			gameEngine->rotate();
		}
		else {
			// �׳� ��� ������
			gameEngine->next(dt, 0); // ���� ����� �� ��Ÿ Ÿ���� �־������.
		}

		// ȭ�� ���
		gameEngine->makeDisplayData();
		display->draw();

		if (gameEngine->state == GameEngine::GameState::GAMEOVER) {
			break; // ���� ������ ���°� ���ӿ����θ� �ݺ��� Ż��
		}

		// ���ӻ��� �Ǻ�
	}

	return 0;
}

