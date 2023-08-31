#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define ENTER 13

#define WIDTH 100
#define HEIGHT 30

#define FPS 1000/60

#define MAX_BULLET 20

#define MAX_STAR 50
#define ET 3

#define TRUE 1
#define FALSE 0

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

#pragma region 전역변수모음
int playerScore = 0; //플레이어 스코어
int ct = 0;
int currentStage = 1;
#pragma endregion

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

#pragma region save

#define DataMax 10000
struct _rank {
	int id;
	char name[11];
	int score;
};
_rank rankData[DataMax];
int rankDataNum = 0;

void initRankData() {
	for (int i = 0; i < DataMax; i++) {
		rankData[i].id = NULL;
		strcpy(rankData[i].name, "\0");
		rankData[i].score = NULL;
	}
}

int cmp(const void* n1, const void* n2) {

	if ((*(_rank*)n1).score <(*(_rank*)n2).score)
		return 1;
	else
		return -1;
}

int rankingUI() {

	FILE* fptr;
	_rank e;

	int key;


	if (0 == fopen_s(&fptr, "gamedata.sav", "r")) {
		for (int i = 0; i < DataMax; i++) {
			rankData[i].id = 0;
			strcpy(rankData[i].name,"");
			rankData[i].score = 0;
			rankDataNum = 0;
		}
		while (!feof(fptr)) {
			fscanf(fptr, "%d %s %d", &e.id, e.name, &e.score);
			rankData[rankDataNum].id = e.id % 10000;
			strcpy(rankData[rankDataNum].name, e.name);
			rankData[rankDataNum].score = e.score;
			rankDataNum++;
		}

		rankDataNum--;
		system("cls");
		gotoXY(20, 10);
		qsort(rankData, rankDataNum, sizeof(_rank), cmp);


		int x = 12;
		int y = 4;
		gotoXY(x, y++);
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");
		gotoXY(x, y++);
		gotoXY(x, y++);
		gotoXY(x, y++);
		gotoXY(x, y++);
		printf("\t순위 \tPlayerID\t플레이어 이름\t\t점수\t\t\t");
		gotoXY(x, y++);
		for (int i = 0; i < rankDataNum; i++) {
			gotoXY(x, y++);
			printf("%5d 위 %8d %16s%21d ",i+1, rankData[i].id, rankData[i].name, rankData[i].score);
		}
		gotoXY(x, y++);
		gotoXY(x, y++);
		gotoXY(x, y++);
		printf("                 주의!: 랭킹은 10위 까지만 표시됩니다!                ");
		gotoXY(x, y++);
		gotoXY(x, y++);
		printf("                                ENTER                                 ");
		gotoXY(x, y++);
		gotoXY(x, y++);
		printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■");



		key = _getch();
		if (key == ENTER) return 0;
	}
	else {
		int x = 26;
		int y = 10;
		gotoXY(x, y++);
		printf("■■■■■■■■■■■■■■■■■■■■■■");
		gotoXY(x, y++);
		printf("■                                        ■");
		gotoXY(x, y++);
		printf("■                < 알림 >                ■");
		gotoXY(x, y++);
		printf("■                                        ■");
		gotoXY(x, y++);
		printf("■     세이브 정보가 존재하지 않습니다.   ■");
		gotoXY(x, y++);
		printf("■                                        ■");
		gotoXY(x, y++);
		printf("■                                        ■");
		gotoXY(x, y++);
		printf("■                 ENTER                  ■");
		gotoXY(x, y++);
		printf("■                                        ■");
		gotoXY(x, y++);
		printf("■■■■■■■■■■■■■■■■■■■■■■");
		
		key = _getch();
		if (key == ENTER) return 0;
	}
	fclose(fptr);

	return 0;
}
#pragma endregion


#pragma region 오브젝트


struct _Player {
	//초기위치
	int x = 10;
	int y = 15;
	int Maxlife; //3
	int life; //3
	int speed; //1
	int damage;
	int shotSpeed; //4
	char shape[3];
	char Bulletshape[3];
	char BulletColor;

	int undieTime = 0;
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
	int hitTest = FALSE;
};
_Bullet bullet[MAX_BULLET];

void bulletErase1(_Bullet *b,const char* str) {
	gotoXY(b->x, b->y);
	printf("%s", str);
}
void bulletMove(_Bullet* b) {
	if (b->exist == 0) {
		b->effectTime = ET;
		b->x = player.x + 2;
		b->y = player.y;
		b->speed = 3;
		b->hitTest = FALSE;
	}
	else {
		textcolor(player.BulletColor, BLACK);
		strcpy(b->shape, player.Bulletshape);
		gotoXY(b->x, b->y);
		printf("%s", b->shape);
		b->x += b->speed;
	}
	//맵 이탈
	if (b->x >= WIDTH - 4||b->hitTest == TRUE) {
		strcpy(b->shape, "");
		b->speed = 0;
	
		//총알 이펙트
		if (b->effectTime == 3) bulletErase1(b,"⊙");
		else if (b->effectTime == 2) bulletErase1(b, "◎");
		else bulletErase1(b, "○");
	
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
		player.Maxlife = 5;
		player.life = 5;
		player.speed = 1;
		player.damage = 5;
		player.shotSpeed = 4;
		strcpy(player.shape, "▶");
		strcpy(player.Bulletshape, "─");
		player.BulletColor = LIGHTCYAN;
		break;
	case hawk:
		player.Maxlife = 3;
		player.life = 3;
		player.speed = 1;
		player.damage = 3;
		player.shotSpeed = 2;
		strcpy(player.shape, "♣");
		strcpy(player.Bulletshape, "ㆍ");
		player.BulletColor = YELLOW;
		break;
	case hera:
		player.Maxlife = 4;
		player.life = 4;
		player.speed = 1;
		player.damage = 7;
		player.shotSpeed = 6;
		strcpy(player.shape, "◆");
		strcpy(player.Bulletshape, "⊙");
		player.BulletColor = WHITE;
		break;
	}
}

int playerHitTest();

void playerAction(int ct) {
	static int num=0;
	//캐릭터 이동
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		if(player.x > player.speed * 2)
		player.x -= player.speed * 2;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		if (player.x < WIDTH - player.speed * 2)
		player.x += player.speed * 2;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		if (player.y > player.speed * 2 + 1)
		player.y -= player.speed;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		if (player.y < HEIGHT - player.speed * 2 - 2)
		player.y += player.speed;
	}
	//캐릭터 그리기
	
	if(player.undieTime == 0)
	textcolor(WHITE, BLACK);
	else 	textcolor(YELLOW, BLACK);

	gotoXY(player.x, player.y);
	printf("%s", player.shape);

	//총쏘기
	
	

	//총알 생성
	if (ct % player.shotSpeed == 0) {
		bullet[num].exist = 1;
		num = (num + 1) % MAX_BULLET;
	}
	
	for (int i = 0; i < MAX_BULLET; i++) {
		bulletMove(&bullet[i]);
	}

	if (playerHitTest() == TRUE && player.undieTime == 0) {
		player.life--;
		player.undieTime = 10;
	}
	if (player.undieTime > 0) {
		player.undieTime--;
		gotoXY(player.x-1, player.y - 1);
		textcolor(YELLOW, BLACK);
		printf("피격!");
	}
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
	printf("					>  GAME START\n");
	puts("");
	printf("					    RANKING\n");
	puts("");
	printf("					     EXIT\n");
	puts("");

	puts("");
	puts("				     ⓒ Made by Ahn Jiwoo");
}
void mainmenu2() {
	printf("					   GAME START\n");
	puts("");
	printf("					>   RANKING\n");
	puts("");
	printf("					     EXIT\n");
	puts("");


	puts("");
	puts("				     ⓒ Made by Ahn Jiwoo");
}

void mainmenu3() {
	printf("					   GAME START\n");
	puts("");
	printf("					    RANKING\n");
	puts("");
	printf("					>    EXIT\n");
	puts("");
	puts("");
	puts("				     ⓒ Made by Ahn Jiwoo");
}

int mainPrint()
{
	readfile("gametitle.txt");
	//

	printf("%s", titlebuffer);
	mainmenu1();

	int key;
	int menu = 1;
	while (1) {
		if (_kbhit()) {
			system("cls");

			key = _getch();

			switch (key) {
			case UP:
				if (menu > 1)
					menu--;
				break;
			case DOWN:
				if (menu < 3)
					menu++;
				break;
			case ENTER:
				switch (menu) {
				case 1:
					return 0;
					break;
				case 2: //랭킹
					rankingUI();
					break;
				case 3:
					exit(0);
				}
			}
			printf("%s", titlebuffer);
			switch (menu) {
			case 1:
				mainmenu1();
				break;
			case 2: //랭킹
				mainmenu2();
				break;
			case 3:
				mainmenu3();
			}
		}
	}
}
#pragma endregion

#pragma region 인게임 UI

void finishDistance() {
	//WIDTH
	
	textcolor(LIGHTGRAY, BLACK);
	gotoXY(3, HEIGHT - 2);
	printf("S ------------------------------------------------------------------------------------------ F");

	gotoXY((WIDTH/100) * ct / 10 + 3, HEIGHT - 2);
	printf("▶");
}

void printLife(int life, int lifeMAX) {
	for (int i = 0; i < lifeMAX; i++)
		if (i < life)
			printf("♥");
		else printf("♡");
}
void DrawInGameUI(int life, int lifeMax, const char* playerName, int score) {
	char str[11];
	if (currentStage == 1)
		strcpy(str,"Stage 1");
	else 
		strcpy(str, "Stage 2");

	textcolor(LIGHTGRAY, BLACK);
	gotoXY(0, 0);
	
	puts(" ==================================================================================================");
	printf("  1P: %s    Life: ", playerName);
	printLife(life, lifeMax);
	printf("    %s %d                           %s", "Score: ", score, str);

	gotoXY(1, 26);
	puts("");
puts(" ==================================================================================================");
	finishDistance();

}
#pragma endregion

void scorePrinter(int score,int x,int y) {
	gotoXY(x, y);
	textcolor(LIGHTRED, BLACK);
	printf("%d", score);
}

#pragma region 적
struct _enemy {
	int x;
	int y;
	//기본적인 정보
	int life;
	int speed;
	int exist = FALSE;
	char shape[3];
	int patterntime;
	char color;
	int dieEvent;
	int score ;

	double angle;

	int EnemyTime;
	float xs;
	float ys;
};
#define urosNum 15
#define callNum 15
#define starManNum 10

_enemy ur[urosNum];
_enemy call[callNum];
_enemy starMan[starManNum];

struct _enemyBullet {
	int x;
	int y;
	int speed;
	int angle;
	int exist = FALSE;
	char shape[3];
	int dieEffect;

	//call용
	float xs;
	float ys;
	int createTime;
};
#define urosBulletMax 20
#define callBulletMax 40
_enemyBullet urosBullet[urosBulletMax];
int urosBulletNum = 0;

_enemyBullet callBullet[callBulletMax];
int callBulletNum = 0;

void DieEffect(int x, int y, const char* str, int color) {
	textcolor(color, BLACK);
	gotoXY(x, y);
	printf("%s", str);
}

void urosBulletAction(_enemyBullet* bul) {
	if (bul->exist == FALSE) {
		bul->x = 0;
		bul->y = 0;
		bul->speed = 0;
		bul->dieEffect = 0;
	}
	else {
		bul->speed = 4;
		if (bul->x > 3)
			bul->x -= bul->speed;
		else {
			bul->speed = 0;
			if(bul->dieEffect == 0)
			bul->dieEffect = 3;
		}
		strcpy(bul->shape, "◈");
		gotoXY(bul->x, bul->y);
		textcolor(YELLOW, BLACK);
		printf("%s", bul->shape);

		if (bul->dieEffect > 0) {
			strcpy(bul->shape, "");
			bul->dieEffect--;
			DieEffect(bul->x, bul->y, "○",YELLOW);
			bul->exist = FALSE;
		}
	}
}


//우로스: 상하로 지그재그 움직이는 몬스터
void urosAction(_enemy* uros){

	//프레임로드
	if (uros->exist == FALSE) {
		uros->dieEvent = 6;
		uros->x = WIDTH - 4;
		uros->y = 3 + rand() % (HEIGHT - 6);
		uros->life = 20;
		uros->speed = 0;
		strcpy(uros->shape, "");
		uros->patterntime = rand()%10;
		uros->color = LIGHTRED;
		uros->score = 100;
		uros->EnemyTime = rand() % 20;
	}
	//엔터프레임
	else if(uros->exist == TRUE && uros->life >0){
		uros->EnemyTime++;
		uros->patterntime++;
		textcolor(uros->color, BLACK);
		strcpy(uros->shape, "◐");
		uros->speed = 1;
		
		if (uros->patterntime % 4 == 0) {
			uros->x -= uros->speed;
			if (uros->patterntime < 20)
				uros->y -= uros->speed;
			else if (uros->patterntime < 40)
				uros->y += uros->speed;
			else uros->patterntime = 0;
		}

		//벽
		while (uros->y < 3) uros->patterntime=20,uros->y++;
		while (uros->y > HEIGHT-4)uros->patterntime = 0, uros->y--;
		if(uros->x <= 2) uros->y = 3 + rand() % (HEIGHT - 6), uros->x = WIDTH - 4;

		//goto
		gotoXY(uros->x, uros->y);
		printf("%s", uros->shape);

		//총알충돌판정
		for (int i = 0; i < MAX_BULLET; i++) {
			if (bullet[i].exist==TRUE&&bullet[i].hitTest == FALSE && bullet[i].x <= uros->x +3 && bullet[i].x >= uros->x - 3 && bullet[i].y <= uros->y + 1 && bullet[i].y >= uros->y - 1)
				bullet[i].hitTest = TRUE, uros->life -= player.damage, bullet[i].x -= 2;
		}
		//총쏘기
		if (uros->EnemyTime % 20 == 0) {
			if (urosBullet[urosBulletNum].exist == FALSE){
				urosBullet[urosBulletNum].x = uros->x - 2;
				urosBullet[urosBulletNum].y = uros->y;
				urosBullet[urosBulletNum].exist = TRUE;
				urosBulletNum = (urosBulletNum + 1) % urosBulletMax;
			}
		}
		//죽음 
	}	else	if (uros->life <= 0) {
		uros->speed = 0;
		strcpy(uros->shape, "");
		uros->dieEvent--;
		if (uros->dieEvent > 0) {
			scorePrinter(uros->score, uros->x-1, uros->y - 1);
			DieEffect(uros->x, uros->y, "○",LIGHTRED);
		}
		else playerScore += uros->score, uros->exist = FALSE;
	}
}

//따릉이: 원을 그리면서 움직이는 몬스터
//우로스: 상하로 지그재그 움직이는 몬스터
void callAction(_enemy* call) {

	//프레임로드
	if (call->exist == FALSE) {
		call->dieEvent = 6;
		call->x = WIDTH - 4;
		call->y = 3 + rand() % (HEIGHT - 6);
		call->life = 20;
		call->speed = 0;
		strcpy(call->shape, "");
		call->patterntime = 0;
		call->color = LIGHTRED;
		call->score = 150;
		call->EnemyTime = rand() % 20+10;

		call->angle = 0;

	}
	//엔터프레임
	else if (call->life > 0) {
		
		call->EnemyTime++;
		call->patterntime++;
		textcolor(call->color, BLACK);
		strcpy(call->shape, "☎");
		call->speed = 1;

		//움직임

		if(call->patterntime%2 ==0)
		call->x -= call->speed;
	
		//벽
		if (call->x < 2)
			call->y = 3 + rand() % (HEIGHT - 6),
			call->x = WIDTH - 4;
		//이동 패턴 변경, 랜덤한 각도로 움직이기

		//goto
		gotoXY(call->x, call->y);
		printf("%s", call->shape);

		//총알충돌판정
		for (int i = 0; i < MAX_BULLET; i++) {
			if (bullet[i].exist == TRUE && bullet[i].hitTest == FALSE && bullet[i].x <= call->x + 3 && bullet[i].x >= call->x - 3 && bullet[i].y <= call->y + 1 && bullet[i].y >= call->y - 1)
				bullet[i].hitTest = TRUE, call->life -= player.damage, bullet[i].x -= 2;
		}
		//총쏘기
		if (call->EnemyTime % 30 == 0) {
			if (callBullet[callBulletNum].exist == FALSE) {
				callBullet[callBulletNum].xs = call->x - 2;
				callBullet[callBulletNum].ys = call->y;
				callBullet[callBulletNum].exist = TRUE;
				callBullet[callBulletNum].angle = 0;
				callBulletNum = (callBulletNum + 1) % callBulletMax;

				callBullet[callBulletNum].xs = call->x - 2;
				callBullet[callBulletNum].ys = call->y;
				callBullet[callBulletNum].exist = TRUE;
				callBullet[callBulletNum].angle = 1;
				callBulletNum = (callBulletNum + 1) % callBulletMax;

				callBullet[callBulletNum].xs = call->x - 2;
				callBullet[callBulletNum].ys = call->y;
				callBullet[callBulletNum].exist = TRUE;
				callBullet[callBulletNum].angle = 2;
				callBulletNum = (callBulletNum + 1) % callBulletMax;
			}
		}
		//죽음 
	}
	else if (call->life <= 0) {
		call->speed = 0;
		strcpy(call->shape, "");
		call->dieEvent--;
		if (call->dieEvent > 0) {
			scorePrinter(call->score, call->x - 1, call->y - 1);
			DieEffect(call->x, call->y, "○", LIGHTRED);
		}
		else playerScore += call->score, call->exist = FALSE;
	}
}

//반짝반짝 작은별
//우로스: 상하로 지그재그 움직이는 몬스터
void starAction(_enemy* starMan) {

	//프레임로드
	if (starMan->exist == FALSE) {
		starMan->dieEvent = 6;
		starMan->x = WIDTH - 4;
		starMan->y = 3 + rand() % (HEIGHT - 6);
		starMan->life = 20;
		starMan->speed = 0;
		strcpy(starMan->shape, "");
		starMan->patterntime = rand() % 10;
		starMan->color = LIGHTRED;
		starMan->score = 200;
		starMan->EnemyTime = 40;
		starMan->xs = 0;
		starMan->ys = 0;
		
	}
	//엔터프레임
	else if (starMan->exist == TRUE && starMan->life > 0) {
		starMan->EnemyTime--;
		starMan->patterntime++;
		textcolor(starMan->color, BLACK);

		if (player.x < starMan->x && starMan->xs > -2)
			starMan->xs -= 0.4;
		else if (player.x > starMan->x && starMan->xs < 2)
			starMan->xs += 0.4;

		if (player.y < starMan->y && starMan->ys > -1)
			starMan->ys -= 0.2;
		else if (player.y > starMan->y && starMan->ys < 1)
			starMan->ys += 0.2;

		starMan->x += starMan->xs;
		starMan->y += starMan->ys;

		while (starMan->y < 3)
			starMan->y++;
		while (starMan->y > HEIGHT-3)
			starMan->y--;

		strcpy(starMan->shape, "★");

		//벽
		if (starMan->EnemyTime > 0)
		{
			if (starMan->x <= 2 || starMan->x > WIDTH - 2) {
				starMan->EnemyTime = 0;
			}

			if (player.undieTime == 0 && player.x <= starMan->x + 3 && player.x >= starMan->x - 3 && player.y >= starMan->y-1 && player.y <= starMan->y +1) {
				player.life--;
				player.undieTime = 10;
				starMan->score = 10;
				starMan->EnemyTime = 0;
			
			}

		}
		//goto
		gotoXY(starMan->x, starMan->y);
		textcolor(LIGHTMAGENTA, BLACK);
		printf("%s", starMan->shape);

		//총알충돌판정
		for (int i = 0; i < MAX_BULLET; i++) {
			if (bullet[i].exist == TRUE && bullet[i].hitTest == FALSE && bullet[i].x <= starMan->x + 3 && bullet[i].x >= starMan->x - 3 && bullet[i].y <= starMan->y + 1 && bullet[i].y >= starMan->y - 1)
				bullet[i].hitTest = TRUE, starMan->life -= player.damage, bullet[i].x -= 2;
		}

		//총쏘기
		//죽음 
		if (starMan->EnemyTime <= 0 && starMan->life > 0)
			starMan->life = 0;
	}
	else if (starMan->life <= 0) {
		starMan->xs = 0;
		starMan->ys = 0;
		strcpy(starMan->shape, "");
		starMan->dieEvent--;
		if (starMan->dieEvent > 0) {
			scorePrinter(starMan->score, starMan->x - 1, starMan->y - 1);
			DieEffect(starMan->x, starMan->y, "○", LIGHTRED);
		}
		else playerScore += starMan->score, starMan->exist = FALSE;
	}
}

void callBulletAction(_enemyBullet* bul) {
	if (bul->exist == FALSE) {
		bul->speed = 0;
		bul->dieEffect = 0;
		bul->createTime = 0;
		bul->xs = 0;
		bul->ys = 0;

	}
	else {
		if (bul->xs > 3 && bul->xs < WIDTH-3 && bul->ys >2  && bul->ys < HEIGHT-4) {
			if (bul->angle == 0)
				bul->xs -= 1 * 2;
			else if (bul->angle == 1)
				bul->xs -= 0.86*2, bul->ys -= 0.25 * 2;
			else if (bul->angle == 2)
				bul->xs -= 0.86 * 2, bul->ys += 0.25 * 2;
		}
		else {
			bul->x = bul->xs;
			bul->y = bul->ys;
			bul->xs = 0;
			bul->ys = 0;
			if (bul->dieEffect == 0)
				bul->dieEffect = 3;
		}
		strcpy(bul->shape, "♬");
		gotoXY(bul->xs, bul->ys);
		textcolor(YELLOW, BLACK);
		printf("%s", bul->shape);

		if (bul->dieEffect > 0) {
			strcpy(bul->shape, "");
			bul->dieEffect--;
			DieEffect(bul->x, bul->y, "○", YELLOW);
			bul->exist = FALSE;
		}
	}
}

void enemySystem(int ct) {
	//urosAction(&ur);
	//ur.exist = TRUE;
	static int num = 0;
	static int num2 = 0;

	if (ct > 10 && ct % 40 == 0) {
		ur[num].exist = TRUE;
		num = (num + 1) % urosNum;
	}
	if (ct > 400 && ct % 50 == 0) {
		call[num2].exist = TRUE;
		num2 = (num2 + 1) % callNum;
	}

	for (int i = 0; i < urosNum; i++)
		urosAction(&ur[i]);

	for (int i = 0; i < callNum; i++)
		callAction(&call[i]);

	for (int i = 0; i < urosBulletMax; i++)
		urosBulletAction(&urosBullet[i]);

	for (int i = 0; i < callBulletMax; i++)
		callBulletAction(&callBullet[i]);

}


void enemySystem2(int ct) {
	//urosAction(&ur);
	//ur.exist = TRUE;
	static int num = 0;
	static int num2 = 0;
	static int num3 = 0;

	if (ct > 10 && ct % 50 == 25) {
		ur[num].exist = TRUE;
		num = (num + 1) % urosNum;
	}
	if (ct > 10 && ct % 50 == 0) {
		call[num2].exist = TRUE;
		num2 = (num2 + 1) % callNum;
	}
	if (ct > 300 && ct % 70 == 0) {
		starMan[num3].exist = TRUE;
		num3 = (num3 + 1) % starManNum;
	}

	for (int i = 0; i < urosNum; i++)
		urosAction(&ur[i]);

	for (int i = 0; i < callNum; i++)
		callAction(&call[i]);

	for (int i = 0; i < starManNum; i++)
		starAction(&starMan[i]);


	for (int i = 0; i < urosBulletMax; i++)
		urosBulletAction(&urosBullet[i]);

	for (int i = 0; i < callBulletMax; i++)
		callBulletAction(&callBullet[i]);

}
#pragma endregion


#pragma region 스테이지 오프닝
void stageOpening() {
	
	int num = 5;
	while (TRUE) {
		system("cls");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		printf("                                           Stage 1\n\n");
		printf("                     --------------------------------------------------------\n\n");
		printf("                                         Mission Goal\n\n");
		printf("                                   적을 최대한 많이 제거하고\n\n");
		printf("                              죽지않고 최종 목적지까지 도달하라!\n\n\n");

		printf("                                      %d초 후 게임 시작...",num);

		Sleep(1000);
		num--;
		if (num == 0) break;

		char key;
		if (_kbhit()) {
			if ((key = _getch()) == ENTER) break;
		}
	}
}

void stageOpening2() {

	int num = 5;
	while (TRUE) {
		system("cls");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		printf("                                           Stage 2\n\n");
		printf("                     --------------------------------------------------------\n\n");
		printf("                                         Mission Goal\n\n");
		printf("                                   적을 최대한 많이 제거하고\n\n");
		printf("                              죽지않고 최종 목적지까지 도달하라!\n\n\n");
		printf("                                  현재 플레이어의 점수: %d\n\n\n",playerScore);
		printf("                                      %d초 후 게임 시작...", num);

		Sleep(1000);
		num--;
		if (num == 0) break;

		char key;
		if (_kbhit()) {
			if ((key = _getch()) == ENTER) break;
		}
	}
}
#pragma endregion

void SaveData(FILE* fileptr, int id, char name[11], int score) {
	fileptr = fopen("gamedata.sav", "a");

	fprintf(fileptr, "%d %s %d\n",id, name, score);

	fclose(fileptr);
}

void gameObjectInit() {
	player.life = player.Maxlife;
	player.x = 10;
	player.y = 15;

	for (int i = 0; i < urosNum; i++)
		ur[i].exist = FALSE;	

	for (int i = 0; i < callNum; i++)
		call[i].exist = FALSE;

	for (int i = 0; i < starManNum; i++)
		starMan[i].exist = FALSE;


	for (int i = 0; i < urosBulletMax; i++)
		urosBullet[i].exist = FALSE;

	for (int i = 0; i < callBulletMax; i++)
		callBullet[i].exist = FALSE;

	for (int i = 0; i < MAX_BULLET; i++)
		bullet[i].exist = FALSE;

}


int main() {
	FILE* save = NULL;
	int player_id;
	int key;
	int gameover = FALSE;
	srand(time(NULL));
	
	Program_init();

	char playerName[11];
	int playerSelectNum;
	//Stage 0 : 메인메뉴
	while (1) {
		mainPrint();
		break;
	}

	//랭킹창

	//Stage 1 : 상태창 입력
	while (1) {
		//상태창 입력
		system("cls");
		stateMenu(playerName);
		break;
	}
	playerScore = 0;
	player_id = time(NULL);
	
	//Stage 2 : 입장전 오프닝
	stageOpening();
	//Stage 2 : 게임 플레이
	ct = 0;
	currentStage = 1;
	while (1) {
		//enterFrame
		Sleep(FPS);
		ct++;
		system("cls");
		playerAction(ct);
		DrawBackground();
		enemySystem(ct);
		DrawInGameUI(player.life, player.Maxlife, playerName, playerScore);
		if (player.life <= 0) {
			gameover = TRUE;
			break;
		}

		if (_kbhit()) {
			if ((key = _getch()) == ENTER) break;
		}
		if (ct >= 940) {//클리어
			break;
		}
	}
	gameObjectInit();
	system("cls");

	if (gameover == TRUE) goto Gameover;
	
	//스테이지3
	stageOpening2();
	//Stage 2 : 게임 플레이
	ct = 0;
	currentStage = 2;
	while (1) {
		//enterFrame
		Sleep(FPS);
		ct++;
		system("cls");
		playerAction(ct);
		DrawBackground();
		enemySystem2(ct);
		DrawInGameUI(player.life, player.Maxlife, playerName, playerScore);
		if (player.life <= 0) {
			gameover = TRUE;
			break;
		}

		if (_kbhit()) {
			if ((key = _getch()) == ENTER) break;
		}
		if (ct >= 940) {//클리어
			break;
		}
	}
	gameObjectInit();
	system("cls");

	if (gameover == TRUE) goto Gameover;

	//모든 스테이지 클리어!
	while (1) {
		system("cls");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		printf("                                        All Stage Clear!!\n\n");
		printf("                          -----------------------------------------------\n\n");
		printf("                                        Player Name: %s\n\n", playerName);
		printf("                                        Your Score: %d\n\n", playerScore);
		puts("                                   Enter를 누르면 종료됩니다..");

		key = _getch();
		if (key  == ENTER) SaveData(save, player_id, playerName, playerScore), exit(0);
	}

	Gameover:
	//게임오버
	while (1) {
		system("cls");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		puts("");
		printf("                                             You Die...\n\n");
		printf("                          -----------------------------------------------\n\n");
		printf("                                        Player Name: %s\n\n", playerName);
		printf("                                        Your Score: %d\n\n", playerScore);
		puts("                                   Enter를 누르면 종료됩니다..");


		key = _getch();
		if (key == ENTER) SaveData(save, player_id, playerName, playerScore), exit(0);
	}

	return 0;
}

int playerHitTest() {
	//우로스와의 충돌
	for (int i = 0; i < urosNum; i++) {
		if (player.x >= ur[i].x - 3 && player.x <= ur[i].x + 3 && player.y == ur[i].y)
			return TRUE;
	}

	for (int i = 0; i < callNum; i++) {
		if (player.x >= call[i].x - 3 && player.x <= call[i].x + 3 && player.y == call[i].y)
			return TRUE;
	}

	for(int i =0;i<urosBulletMax;i++)
		if (urosBullet[i].dieEffect == 0 &&player.x >= urosBullet[i].x - 3 && player.x <= urosBullet[i].x + 3 && player.y == urosBullet[i].y) {
			urosBullet[i].speed = 0;
			urosBullet[i].dieEffect = 3;
			return TRUE;
		}

	for (int i = 0; i < callBulletMax; i++)
		if (callBullet[i].dieEffect == 0 && player.x >= callBullet[i].xs - 3 && player.x <= callBullet[i].xs + 3 && player.y == callBullet[i].ys) {
			callBullet[i].speed = 0;
			callBullet[i].dieEffect = 3;
			return TRUE;
		}


	return FALSE;
}