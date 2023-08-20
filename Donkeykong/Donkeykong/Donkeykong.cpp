#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>

#define BLOCK_MAX 20
#define LEFT 75
#define RIGHT 77
#define UP 32 //스페이스바

#pragma region 게임 시스템 (좌표이동)
void gotoXY(int x, int y)
{
	//x,y 좌표설정
	COORD position = { x,y };

	//커서 이동
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

void CursorView()
{
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1; //커서 굵기 (1 ~ 100)
	cursorInfo.bVisible = FALSE; //커서 Visible TRUE(보임) FALSE(숨김)
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
#pragma endregion

#pragma region 오브젝트 (캐릭터, 벽돌, 적)
struct player {
	int x = 15;
	int y = 2;
	int gravity = 0; //중력
	char shape[3] = "●";
};

struct block {
	int x = 10;
	int y = 20;
	char shape[3] = "■";
};

struct player player;
struct block block[BLOCK_MAX];
#pragma endregion

//맵 초기화 함수
void InitMap() {
	for (int i = 0; i < BLOCK_MAX; i++) {
		block[i].x += i * 2;
	}
}
void DrawPlayer() {
	int trigger = 0; //벽 충돌트리거
	//플레이어 물리엔진
	//
	gotoXY(player.x, player.y);
	printf("%s", player.shape);


	for (int i = 0; i < BLOCK_MAX; i++) {
		struct block b = block[i];
		if (b.y - 1 == player.y && b.x - 1 <= player.x && b.x + 1 >= player.x) {
			trigger = 1;
			break;
		}
	}


	if (_kbhit()) {
		int k = _getch();
		switch (k) {
		case UP: if (trigger == 1) player.gravity = -5, trigger = 0;
			break;
		case LEFT: player.x -= 2;
			break;
		case RIGHT: player.x += 2;
			break;
		}
	}


	if (trigger == 0) {
		player.gravity++;
		if (player.gravity > 0)
			player.y++;
		else player.y--;
	}
	//키입력


}
void DrawMap() {
	for (int i = 0; i < BLOCK_MAX; i++) {
		gotoXY(block[i].x, block[i].y);
		printf("%s", block[i].shape);
	}
}
void Update() {
	system("cls");
	DrawPlayer();
	DrawMap();
}
int main() {
	//초기화
	InitMap();
	CursorView();
	while (1) {
		Update();
	}
	return 0;
}