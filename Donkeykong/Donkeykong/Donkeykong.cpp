#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 25

#define FPS 1000/30

#define BLOCK_MAX 20
#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define MAX_BULLET 10
#define MAX_STAR 50

#pragma region 시스템
void textcolor(int foreground, int background)
{
	int color = foreground + background * 16;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

#pragma endregion

#pragma region  색 상수
// color상수 지정 
#define BLACK 0 
#define BLUE 1 
#define GREEN 2 
#define CYAN 3 
#define RED 4 
#define MAGENTA 5 
#define BROWN 6 
#define LIGHTGRAY 7 
#define DARKGRAY 8 
#define LIGHTBLUE 9 
#define LIGHTGREEN 10 
#define LIGHTCYAN 11 
#define LIGHTRED 12 
#define LIGHTMAGENTA 13 
#define YELLOW 14 
#define WHITE 15
#pragma endregion

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
	system("mode con cols=80 lines=25");
	CursorView(0);
}
#pragma endregion

#pragma region 오브젝트 (캐릭터, 벽돌, 적)
struct _Player {
	int x = 10;
	int y = 12;
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

void starMove(_Star* s) {
	if((*s).x >= player.x - 2 && (*s).x <= player.x+2 && (*s).y == player.y) strcpy((*s).shape, "");
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
#pragma region 총알관련
void bulletErase1(_Bullet* b) {
	gotoXY((*b).x, (*b).y);
	puts("◎");
}
#pragma endregion


void bulletMove(_Bullet* b) {
	if ((*b).exist == 0) {
		(*b).x = player.x+2;
		(*b).y = player.y;
	}
	else {
		textcolor(LIGHTCYAN, BLACK);
		strcpy((*b).shape, "━");
		gotoXY((*b).x, (*b).y);
		printf("%s", (*b).shape);
		(*b).x += 2;
	}
	if ((*b).x >= WIDTH - 2)bulletErase1(b), (*b).exist = 0;
}
void playerAction(int ct) {
	char key;
	if (_kbhit()) {
		key = _getch();

		switch (key)
		{
		case UP:
			if(player.y > 1)
			player.y -= player.speed;
			break;
		case DOWN:
			if (player.y < HEIGHT-2)
			player.y += player.speed;
			break;
		case LEFT:
			if(player.x > 2)
			player.x -= player.speed * 2;
			break;
		case RIGHT:
			if (player.x < WIDTH-4)
			player.x += player.speed * 2;
			break;
			
		}
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
	int ct = 0;
	Program_init();
	
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

