#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <string.h>

#define UP 72
#define DOWN 80
#define ENTER 13


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
	while (1){
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
			}
		}
	} 	
}