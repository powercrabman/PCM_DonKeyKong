#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <string.h>
#include "gamesystem.h"
#include "mainmenu.h"
#include "statemenu.h"

#define WIDTH 100
#define HEIGHT 30

#define BLOCK_MAX 20
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define MAX_BULLET 15
#define MAX_STAR 50

int ct = 0;

#pragma region 오브젝트 (캐릭터, 벽돌, 적)
struct _Player {
	int x = 10;
	int y = 15;
	int speed = 1;
	char shape[3] = "▶";
};
_Player player;

struct _Bullet {
	int x = 0;
	int y = 0;
	int speed = 4;
	char shape[3] = "ㆍ";
	bool exist = 0;
#define ET 2
	int effectTime = ET;
};
_Bullet bul[MAX_BULLET];

struct _Star {
	int x = rand() % (WIDTH - 8) + 4;
	int y = rand()%(HEIGHT-4) +2;
	char shape[3] = "ㆍ";
	bool exist = 0;
};
_Star star[MAX_STAR];
#pragma endregion

#pragma region 총알관련

void bulletErase1(_Bullet* b) {
	int t = ct;
	gotoXY((*b).x, (*b).y);
	puts("◎");
	if (b->effectTime <=ET/2) {
		gotoXY((*b).x, (*b).y);
		puts("○");
	}
}
void bulletMove(_Bullet* b) {
	if ((*b).exist == 0) {
		b->effectTime = ET;
		(*b).x = player.x +2;
		(*b).y = player.y;
		b->speed = 3;
	}
	else {
		textcolor(LIGHTCYAN, BLACK);
		strcpy((*b).shape, "━");
		gotoXY((*b).x, (*b).y);
		printf("%s", (*b).shape);
		(*b).x += b->speed;
	}
	//맵 이탈
	if ((*b).x >= WIDTH - 4) {
		strcpy((*b).shape, "0");
		b->x = WIDTH - 4;
		b->speed = 0;
		bulletErase1(b);
		b->effectTime--;
	}
	if(b->effectTime <= 0)
		b->exist = 0;
}
#pragma endregion

void starMove(_Star* s) {
	if ((*s).x >= player.x - 2 && (*s).x <= player.x + 2 && (*s).y == player.y) strcpy((*s).shape, "");
	else  strcpy((*s).shape, "ㆍ");
	if ((*s).x < 2) {
		(*s).x = WIDTH - 2;
		(*s).y = rand() % (HEIGHT - 2) + 1;
	}
	gotoXY((*s).x, (*s).y);
	textcolor(DARKGRAY, BLACK);
	printf("%s", (*s).shape);
	(*s).x -= 1;
}

void playerAction(int ct) {
	//캐릭터 이동
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) { 
			player.x-=2;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) { 
			player.x += 2;
		}
		if (GetAsyncKeyState(VK_UP) & 0x8000) { 
			player.y--;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) { 
			player.y++;
		}
	//캐릭터 그리기
	textcolor(WHITE, BLACK);
	gotoXY(player.x, player.y);
	printf("%s", player.shape);

	//총쏘기
	for (int i = 0; i < MAX_BULLET; i++)
		bulletMove(&bul[i]);

	bul[ct/4 % MAX_BULLET].exist = 1;
}
void DrawBackground() {
	for (int i = 0; i < MAX_STAR; i++)
		starMove(&star[i]);
}
int main() {
	srand(time(NULL));
	
	Program_init();
	//Stage 0 : 메인메뉴
	while (1) {
		mainPrint();
		break;
	}

	//Stage 1 : 상태창 입력
	while (1) {
		//상태창 입력
		system("cls");
		stateMenu();
	}

	//Stage 1 : 게임 플레이
	while (1) {
		//enterFrame
		Sleep(FPS);
		ct++;
		system("cls");
		playerAction(ct);
		DrawBackground();
	}

	return 0;
}

