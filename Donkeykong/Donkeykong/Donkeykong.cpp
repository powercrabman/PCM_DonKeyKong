#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <string.h>

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ENTER 13

#define WIDTH 100
#define HEIGHT 30

#define FPS 1000/30

#define MAX_BULLET 20

#define MAX_STAR 50
#define ET 2

#pragma region  색 상수
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



int ct = 0;

//텍스트 색 변경
void textcolor(int foreground, int background)
{
	int color = foreground + background * 16;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

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

#pragma region 오브젝트


struct _Player {
	//초기위치
	int x = 10;
	int y = 15;
	int life; //3
	int speed; //1
	int damage;
	int shotSpeed; //4
	char shape[3];
	char Bulletshape[3];
	char BulletColor;
};
_Player player;

struct _Star {
	int x = rand() % (WIDTH - 8) + 4;
	int y = rand() % (HEIGHT - 4) + 2;
	char shape[3] = "ㆍ";
	bool exist = 0;
};
_Star star[MAX_STAR];


struct _Bullet {
	int x = 0;
	int y = 0;
	int speed = 4;
	char shape[3];
	bool exist = 0;

	int effectTime = ET;
};
_Bullet bul[MAX_BULLET];


void bulletErase1(struct _Bullet* b) {
	gotoXY((*b).x, (*b).y);
	puts("◎");
	if (b->effectTime <= ET / 2) {
		gotoXY((*b).x, (*b).y);
		puts("○");
	}
}
void bulletMove(struct _Bullet* b) {
	if ((*b).exist == 0) {
		b->effectTime = ET;
		(*b).x = player.x + 2;
		(*b).y = player.y;
		b->speed = 3;
	}
	else {
		textcolor(player.BulletColor, BLACK);
		strcpy((*b).shape, player.Bulletshape);
		gotoXY((*b).x, (*b).y);
		printf("%s", (*b).shape);
		(*b).x += b->speed;
	}
	//맵 이탈
	if ((*b).x >= WIDTH - 4) {
		strcpy((*b).shape, "");
		b->x = WIDTH - 4;
		b->speed = 0;
		bulletErase1(b);
		b->effectTime--;
	}
	if (b->effectTime <= 0)
		b->exist = 0;
}

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

void DrawBackground() {
	for (int i = 0; i < MAX_STAR; i++)
		starMove(&star[i]);
}

#pragma endregion

#pragma region 캐릭터
enum charSelect { dead, hawk, hera };

void playerSelect(int num) {
	//num: 캐릭터 설정 정수

	switch (num) {
	case dead:
		player.life = 5;
		player.speed = 1;
		player.damage = 4;
		player.shotSpeed = 4;
		strcpy(player.shape, "▶");
		strcpy(player.Bulletshape, "─");
		player.BulletColor = LIGHTCYAN;
		break;
	case hawk:
		player.life = 3;
		player.speed = 1;
		player.damage = 3;
		player.shotSpeed = 2;
		strcpy(player.shape, "♣");
		strcpy(player.Bulletshape, "ㆍ");
		player.BulletColor = YELLOW;
		break;
	case hera:
		player.life = 4;
		player.speed = 1;
		player.damage = 5;
		player.shotSpeed = 6;
		strcpy(player.shape, "◆");
		strcpy(player.Bulletshape, "●");
		player.BulletColor = LIGHTRED;
		break;
	}
}



void playerAction(int ct) {
	//캐릭터 이동
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		if(player.x < WIDTH-4)
		player.x -= player.speed * 2;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		if (player.x > 4)
		player.x += player.speed * 2;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		player.y -= player.speed;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		player.y += player.speed;
	}
	//캐릭터 그리기
	textcolor(WHITE, BLACK);
	gotoXY(player.x, player.y);
	printf("%s", player.shape);

	//총쏘기
	for (int i = 0; i < MAX_BULLET; i++)
		bulletMove(&bul[i]);

	bul[ct / player.shotSpeed % MAX_BULLET].exist = 1;
}
#pragma endregion

#pragma region statemenu
#pragma region 캐릭터 선택 ui
void selectMenu3() {
	puts("");
	puts("");
	puts("");
	puts("   ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒    ◆                                                              ▒");
	puts("   ▒      ▶ Triangle     ▒                                                                    ▒");
	puts("   ▒                      ▒    기체 이름: Diamond                                              ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒    내구도: ■ ■ ■ ■                                             ▒");
	puts("   ▒       ♣  Clover     ▒                                                                    ▒");
	puts("   ▒                      ▒    스피드: ■ ■ ■                                                ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒    파괴력: ■ ■ ■ ■ ■                                          ▒");
	puts("   ▒	 → ◆ Diamond    ▒                                                                    ▒");
	puts("   ▒                      ▒    연사력: ■ ■ ■                                                ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");

}
void selectMenu2() {
	puts("");
	puts("");
	puts("");
	puts("   ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒    ♣                                                              ▒");
	puts("   ▒      ▶ Triangle     ▒                                                                    ▒");
	puts("   ▒                      ▒    기체 이름: Clover                                               ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒    내구도: ■ ■ ■                                                ▒");
	puts("   ▒     → ♣  Clover    ▒                                                                    ▒");
	puts("   ▒                      ▒    스피드: ■ ■ ■ ■                                             ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒    파괴력: ■ ■ ■                                                ▒");
	puts("   ▒	   ◆ Diamond     ▒                                                                    ▒");
	puts("   ▒                      ▒    연사력: ■ ■ ■ ■ ■                                          ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");

}
void selectMenu1() {
	puts("");
	puts("");
	puts("");
	puts("   ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒    ▶                                                              ▒");
	puts("   ▒    → ▶ Triangle    ▒                                                                    ▒");
	puts("   ▒                      ▒    기체 이름: Triangle                                             ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒    내구도: ■ ■ ■ ■ ■                                          ▒");
	puts("   ▒      ♣  Clover      ▒                                                                    ▒");
	puts("   ▒                      ▒    스피드: ■ ■ ■                                                ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒    파괴력: ■ ■ ■ ■                                             ▒");
	puts("   ▒	   ◆ Diamond     ▒                                                                    ▒");
	puts("   ▒                      ▒    연사력: ■ ■ ■                                                ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒                      ▒                                                                    ▒");
	puts("   ▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒");

}
#pragma endregion


void nameing(char name[]) {
	CursorView(true);
	char buffer[100];
	scanf("%s", &buffer);
	strcpy(name, buffer);
	CursorView(false);
}

void stateMenu2(char buffer[]) {
	puts("                                       ");
	puts("     ==================================================     ");
	puts("               플레이어의 이름을 기록해주세요!     ");
	puts("     ==================================================     ");
	puts("                                       ");
	puts("                                       ");
	printf("    >> %s",buffer);
	puts("");
	puts("");
	puts("");
	printf("    플레이어의 이름: %s", buffer);
}

int stateMenu(char buffer[]) {
	puts("                                       ");
	puts("     ==================================================     ");
	puts("               플레이어의 이름을 기록해주세요!     ");
	puts("     ==================================================     ");
	puts("                                       ");
	puts("                                       ");
	printf("    >> ");

	nameing(buffer);

	system("cls");
	stateMenu2(buffer);

	//기체 설정
	selectMenu1();
	int key;
	int num = 0;
	while (1) {
		if (_kbhit()) {
			key = _getch();

			switch (key) {
			case DOWN:
				if(num <2)
				num++;
				break;
			case UP:
				if (num > 0)
					num--;
				break;
			case ENTER:
				playerSelect(num);
				return 1;
			}

			system("cls");
			stateMenu2(buffer);
			switch (num) {
			case 0:
				selectMenu1();
				break;
			case 1:
				selectMenu2();
				break;
			case 2:
				selectMenu3();
				break;
			}
		}
	}

}

#pragma endregion

#pragma region 메인메뉴



#define MAX 20000
char titlebuffer[MAX] = { 0, };

//메인메뉴 출력

void readfile(const char* str) {
	FILE* readFile;
	readFile = fopen(str, "r");

	fread(titlebuffer, 1, MAX, readFile);

	fclose(readFile);
}

void mainmenu1() {
	puts("");
	puts("");
	puts("");
	printf("					>  GAME START\n");
	puts("");
	printf("					      EXIT\n");
	puts("");
	puts("");
	puts("");
	puts("");
	puts("");
	puts("");
	puts("				      ⓒ Made by Ahn Jiwoo");
}
void mainmenu2() {
	puts("");
	puts("");
	puts("");
	printf("					   GAME START\n");
	puts("");
	printf("					>     EXIT\n");
	puts("");
	puts("");
	puts("");
	puts("");
	puts("");
	puts("");
	puts("				      ⓒ Made by Ahn Jiwoo");
}


int mainPrint()
{
	readfile("gametitle.txt");
	//

	printf("%s\n\n\n", titlebuffer);
	mainmenu1();

	int key;
	int menu = 1;
	while (1) {
		if (_kbhit()) {
			system("cls");
			printf("%s\n\n\n", titlebuffer);

			key = _getch();

			switch (key) {
			case UP:
				mainmenu1();
				menu = 1;
				break;
			case DOWN:
				mainmenu2();
				menu = 2;
				break;
			case ENTER:
				if (menu == 1) return 0;
				else exit(0);
			default:
				if (menu == 1) mainmenu1();
				else mainmenu2();
			}
		}
	}
}
#pragma endregion


int main() {
	srand(time(NULL));
	
	Program_init();

	char playerName[11];
	int playerScore = 0;
	int playerSelectNum;
	//Stage 0 : 메인메뉴
	while (1) {
		mainPrint();
		break;
	}

	//Stage 1 : 상태창 입력
	while (1) {
		//상태창 입력
		system("cls");
		stateMenu(playerName);
		break;
	}

	playerScore = 0;
	//Stage 2 : 게임 플레이
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

