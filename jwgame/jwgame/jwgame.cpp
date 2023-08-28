#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <string.h>

#define WIDTH 100
#define HEIGHT 30

#pragma region 게임 시스템 (좌표이동)
void gotoXY(int x, int y)
{
	//x,y 좌표설정
	COORD position = { x,y };

	//커서 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

void CursorView(bool show)//커서 깜빡임
{
	HANDLE hConsole;
	CONSOLE_CURSOR_INFO ConsoleCursor;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	ConsoleCursor.bVisible = show;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(hConsole, &ConsoleCursor);
}

void Program_init() {
	system("mode con cols=100 lines=30");
	CursorView(0);
}
#pragma endregion

#pragma region 게임시스템 (아이템 흭득 큐)

#define QMAX 4

enum itemcode { NoItem = 0, ShotSpeedUp, DamageUp, ShotUpgrade, Satellite };


struct _itemQ {
	int itemQ[QMAX] = { 0, };
	int rear = 0;
	int front = 0;
};

int is_full(_itemQ* Q) {
	return (Q->rear + 1) % QMAX == Q->front;
}

int is_empty(_itemQ* Q) {
	return Q->front == Q->rear;
}

int itemGet(_itemQ* Q) {
	if (is_empty(Q))//언더플로우
		return 0;

	int output = Q->itemQ[Q->front];
	Q->front = (Q->front + 1) % QMAX;

	return output;
}

int itemPut(_itemQ* Q, int itemKey) {
	if (is_full(Q)) //오버플로우
		return 0;

	Q->itemQ[Q->rear] = itemKey;
	Q->rear = (Q->rear + 1) % QMAX;
	return 1;
}


#pragma endregion

void printLife(int life, int lifeMAX) {
	for (int i = 0; i < lifeMAX; i++)
		if (i < life)
			printf("♥");
		else printf("♡");
}
void printItem(_itemQ* Q) {
	for (int i = Q->front; i != Q->rear; i = (i + 1) % QMAX) {
		switch (Q->itemQ[i]){
		case NoItem:
			printf("NoItem ");
			break;
		case ShotSpeedUp:
			printf("ShotSpeedUp ");
			break;
		case ShotUpgrade:
			printf("ShotUpgrade ");
			break;
		case DamageUp:
			printf("DamageUp ");
			break;
		case Satellite:
			printf("Satellite ");
			break;
			}
	}
}

void DrawInGameUI(int life,int lifeMax, const char* playerName, int score, _itemQ* Q) {
	puts(" ==================================================================================================");
	printf("  1P: %s    Life: ",playerName);
	printLife(life, lifeMax);
	printf("    %s %d","Score: ", score);

	gotoXY(1, 27);
	printf("Item: ");
	printItem(Q);
	puts("");
	puts(" ==================================================================================================");
	
}

void main() {
	Program_init();
	int life = 5;
	int lifeMAX = 5;
	int score = 0;
	const char* name = "Ahnjiwoo";
	CursorView(false);
	clock_t ct = clock();

	_itemQ iQueue;

	int key;
	while (1) {
		system("cls");
		DrawInGameUI(life, lifeMAX, name, score, &iQueue);
		Sleep(1000 / 30);
		if (_kbhit()) {
			key = _getch();
			switch (key) {
				case 'a':
					life--;
					break;
				case 'd':
					life++;
					break;
				case 'w':
					score+=10;
					break;
				case 's':
					score -= 10;
					break;
				case 'f':
					itemPut(&iQueue, rand() % 5);
					break;
				case 'g':
					itemGet(&iQueue);
					break;
			}
		}
	}
}