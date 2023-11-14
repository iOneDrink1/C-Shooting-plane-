#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "ImageLayer.h"

#define SPECIAL1 0XE0 // 방향키 설정
#define SPECIAL2 0x00
#define SPACE 0x20
#define UP 0x48
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d
#define WIDTH 62
#define HEIGHT 48
#define UX 30  
#define UY 45  

int called = 0;
int oldx = UX, oldy = UY; // 플레이어의 old 좌표
int newx = UX, newy = UY; //플레이어의 new 좌표
int keep_moving = 1;
int Delay = 10;
int frame_count = 0;
int p1_frame_sync = 4;

#define BLACK 0 // 색
#define BLUE1 1
#define GREEN1 2
#define CYAN1 3
#define RED1 4
#define MAGENTA1 5
#define YELLOW1 6
#define GRAY1 7
#define GRAY2 8
#define BLUE2 9
#define GREEN2 10
#define CYAN2 11
#define RED2 12
#define MAGENTA2 13
#define YELLOW2 14
#define WHITE 15

#define MAXBULLET 6 // 최대 총알 수
#define TRUE 1
#define FALSE 0
#define SPACE 0x20 // SPACE 바

struct { // 총알
	int exist;
	int x, y;
}Bullet[MAXBULLET];

void removeCursor(void) { // 콘솔창의 커서 숨기기
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	cursorInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

#define MAXENEMY 6

struct { // 적 
	int exist;
	int x, y;
	int number;
	int move;
	int type;
}Enemy[MAXENEMY];

char* Enemyunit[] = { "⊙★⊙","＠▒＠","♨▣♨","◐▼◑" }; // 적 유형

void textcolor(int text, int bg) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), text + bg * 16);
}

void gotoxy(int x, int y)
{
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
} //콘솔창의 커서 이동 gotoxy 함수

void playerdraw(int x, int y) { // 플레이어 이동 - 생성
	textcolor(GREEN1, BLACK);
	gotoxy(x, y);
	printf("<=★=>");
}
void playererase(int x, int y) { // 플레이어 이동 - 삭제
	gotoxy(x, y);
	printf("      ");
}

void init_game() { // 게임 시작할때 초기세팅
	system("cls");
	removeCursor();
}
void playermove(unsigned char ch) { // 플레이어 이동 처리
	int move_flag = 0;
	static unsigned char last_ch = 0;

	if (called == 0) {
		removeCursor();
		playerdraw(oldx, oldy);
		called = 1;
	}
	if (keep_moving && ch == 0)
		ch = last_ch;
	last_ch = ch;

	switch (ch) {
	case UP:
		if (oldy > 25)
			newy = oldy - 1;
		move_flag = 1;
		break;
	case DOWN:
		if (oldy < HEIGHT - 3)
			newy = oldy + 1;
		move_flag = 1;
		break;
	case LEFT:
		if (oldx > 2)
			newx = oldx - 1;
		move_flag = 1;
		break;
	case RIGHT:
		if (oldx < WIDTH - 1)
			newx = oldx + 1;
		move_flag = 1;
		break;
	}
	if (move_flag) {
		playererase(oldx, oldy);
		playerdraw(newx, newy);
		oldx = newx;
		oldy = newy;
	}
}

void DrawBullet(int i) { // 총알 생성
	textcolor(GREEN1, BLACK);
	gotoxy(Bullet[i].x, Bullet[i].y);
	printf("│");
}

void EraseBullet(int i) { // 총알 삭제
	gotoxy(Bullet[i].x, Bullet[i].y);
	printf(" ");
}
void bulletmove() { // 총알 이동하는 함수
	int i;

	for (i = 0; i < MAXBULLET; i++) {
		if (Bullet[i].exist == TRUE) {
			EraseBullet(i);
			if (Bullet[i].y == 0) {
				Bullet[i].exist = FALSE;
			}
			else {
				Bullet[i].y--;
				DrawBullet(i);
			}
		}
	}
}

void showenemy() {
	int i, location, direct, hieght;
	location = rand() % 2;
	direct = 5 + rand() % 55;
	hieght = 1 + rand() % 8;
	for (i = 0; i < MAXENEMY && Enemy[i].exist == TRUE; i++) {}
	if (i != MAXENEMY) {
		if (location == 1) {
			Enemy[i].x = direct;
			Enemy[i].y = hieght;
			Enemy[i].move = 1;
		}
		else {
			Enemy[i].x = direct;
			Enemy[i].y = hieght;
			Enemy[i].move = -1;
		}
		Enemy[i].type = rand() % 4;
		Enemy[i].exist = TRUE;
	}
}

void Enemymove() {
	int i;
	for (i = 0; i < MAXENEMY; i++) {
		if (Enemy[i].exist == TRUE) {
			if (Enemy[i].type == -1) {
				gotoxy(Enemy[i].x - 1, Enemy[i].y);
				printf("          ");
				Enemy[i].exist = FALSE;
				continue;
			}
			if (Enemy[i].x > 52) {
				Enemy[i].x -= 1;
				Enemy[i].move = -1;


			}
			else if (Enemy[i].x <= 3) {

				Enemy[i].move = 1;
				Enemy[i].x += 1;
				gotoxy(Enemy[i].x, Enemy[i].y);
				printf(" ");
			}
			else {
				Enemy[i].x += Enemy[i].move;
				gotoxy(Enemy[i].x, Enemy[i].y);
				printf(Enemyunit[Enemy[i].type]);
				printf(" ");
			}
		}
	}
}
void gamestart() { // 게임을 실행하면 작동되어야 하는 것들
	unsigned char ch;
	int i;
	init_game();

	while (1) {
		if (_kbhit() == 1) { // 키가 눌렸을때
			ch = _getch(); // 어떤 키가 눌렸는지 받기
			if (ch == SPECIAL1 || ch == SPECIAL2) { 
				ch = _getch();
				switch (ch) {
				case UP: case DOWN: case LEFT: case RIGHT:
					playermove(ch);
					if (frame_count % p1_frame_sync == 0)
						playermove(0);
					break;
				default:
					if (frame_count % p1_frame_sync == 0)
						playermove(0);
				}
			}
			if (ch == SPACE) {
				
				for (i = 0; i < MAXBULLET && Bullet[i].exist == TRUE; i++) {}
				if (i != MAXBULLET) {
					Bullet[i].x = newx + 1;
					Bullet[i].y = newy - 1;
					Bullet[i].exist = TRUE;
				}
				
			}
		}
		else {
			playermove(0);

		}
		if (frame_count % 5 == 0) {
			showenemy();
			Enemymove();


		}
		bulletmove();
		Sleep(Delay); 
		frame_count++;

	}

}

void help() { // 도움말
	system("cls");
	gotoxy(33, 22);
	printf("\n");
	system("pause");
	system("cls");
}

void StartMenu() { // 시작 메뉴 구현
	int a;
	while (1) {
		gotoxy(26, 15);
		printf("게임시작(1)");


		gotoxy(26, 25);
		printf("도움말(2)");

		gotoxy(26, 35);
		printf("종료(3)\b");

		gotoxy(26, 45);
		printf("번호를 입력하세요 : ");
		scanf_s("%d", &a);
		if (a == 1) {
			gamestart();
			break;
		}
		else if (a == 2) {
			help();
		}
		else if (a == 3) {
			printf("Bye");
			break;
		}
	}
}





int main() {
	system("mode con: cols=68 lines=50");
	srand(time((unsigned int)NULL));
	textcolor(YELLOW2, BLACK);
	removeCursor();
	gotoxy(33, 22);
	printf("ShootingGame");



	Sleep(1000);
	system("cls");
	StartMenu();
	Sleep(500);
	
	
	gotoxy(26, 40);
	printf("번호 입력 : ");


	Sleep(50000);
}