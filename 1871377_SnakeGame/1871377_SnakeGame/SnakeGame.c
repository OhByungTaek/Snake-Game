#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// 색상 정의
#define BLACK	0
#define BLUE1	1
#define GREEN1	2
#define CYAN1	3
#define RED1	4
#define MAGENTA1 5
#define YELLOW1	6
#define GRAY1	7
#define GRAY2	8
#define BLUE2	9
#define GREEN2	10
#define CYAN2	11
#define RED2	12
#define MAGENTA2 13
#define YELLOW2	14
#define WHITE	15

#define STAR '*'
#define BLANK '.' // ' ' 로하면 흔적이 지워진다 

#define ESC 0x1b //  ESC 누르면 종료

#define SPECIAL1 0xe0 // 특수키는 0xe0 + key 값으로 구성된다.
#define SPECIAL2 0x00 // keypad 경우 0x00 + key 로 구성된다.

#define UP  0x48 // Up key는 0xe0 + 0x48 두개의 값이 들어온다.
#define DOWN 0x50
#define LEFT 0x4b
#define RIGHT 0x4d

#define WIDTH 80
#define HEIGHT 25
#define MAP_X 3
#define MAP_Y 2

#define SNAKE_LENGTH 6
#define SNAKE_LENGTH_MAX	20
COORD snake[SNAKE_LENGTH_MAX]; // snake 의 좌표를 gotoxy2()에서 사용하기 위함.
int snake_length = SNAKE_LENGTH;
int Delay = 110; 
int map[HEIGHT][WIDTH] = { 1,0,1,0,1,0,1,0,1 };
int score = 0;
int golds[WIDTH][HEIGHT] = { 0 }; // 1이면 Gold 있다는 뜻
int items[WIDTH][HEIGHT] = { 0 }; // 1이면 item 있다는 뜻
int goldinterval = 4; // GOLD 표시 간격
int iteminterval = 15; // 아이템 표시 간격
int keep_moving;

void gotoxy(int x, int y) //내가 원하는 위치로 커서 이동
{
	COORD pos; // Windows.h 에 정의
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void gotoxy2(COORD pos)
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void textcolor(int fg_color, int bg_color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg_color | bg_color << 4);
}

void removeCursor(void) { // 커서를 안보이게 한다
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}
void showCursor(void) { // 커서를 보이게 한다
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}
void showsnake() {
	int i;
	textcolor(WHITE, BLACK);
	gotoxy2(snake[0]); printf("◈"); // 머리를 먼저 그리고
	for (i = 1; i < snake_length; i++) {
		gotoxy2(snake[i]);
		printf("□"); // 몸통을 그린다.
	}
	gotoxy2(snake[snake_length - 1]); printf("☆");
}
void movesnake(int x, int y)
{
	int i;

	gotoxy2(snake[snake_length - 1]);//꼬리좌표
	printf("  "); // 꼬리를 지운다
	for (i = snake_length - 1; i >= 1; i--) { // 전체 좌표를 한칸씩 뒤로 이동한다.
		snake[i] = snake[i - 1];
	}
	snake[0].X = x; snake[0].Y = y; // 새로운 머리 좌표를 추가한다. 머리만 늘어난다.
	showsnake();
}

void putstar(int x, int y, char ch)
{
	gotoxy(x, y);
	putchar(ch);
}

void erasestar(int x, int y)
{
	gotoxy(x, y);
	putchar(' ');//(BLANK);
}
// 화면 지우기고 원하는 배경색으로 설정한다.
void cls(int bg_color, int text_color)
{
	char cmd[100];
	system("cls");
	sprintf(cmd, "COLOR %x%x", bg_color, text_color);
	system(cmd);

}
int block_x = 40;
int block_y = 19;
void init_block1()
{
	textcolor(BLACK, WHITE);
	map[block_y][block_x + 2] = 1;		//장애물 1번
	map[block_y + 1][block_x + 2] = 1;
	map[block_y + 2][block_x + 2] = 1;
	map[block_y + 3][block_x + 2] = 1;
	map[block_y + 4][block_x + 2] = 1;
	map[block_y + 5][block_x + 2] = 1;
	map[block_y + 6][block_x + 2] = 1;

	map[block_y - 18][block_x-10] = 1;	//장애물 2번
	map[block_y - 17][block_x-10] = 1;
	map[block_y - 16][block_x-10] = 1;
	map[block_y - 15][block_x-10] = 1;
	map[block_y - 14][block_x-10] = 1;
	map[block_y - 13][block_x-10] = 1;

	map[block_y - 10][block_x -25] = 1;	//장애물 3번	안댐
	map[block_y - 9][block_x - 25] = 1;
	map[block_y - 8][block_x - 25] = 1;
	map[block_y - 7][block_x - 25] = 1;
	map[block_y - 6][block_x - 25] = 1;
	map[block_y - 5][block_x - 25] = 1;

	map[block_y - 3][block_x + 25] = 1;	//장애물 4번	안댐
	map[block_y - 2][block_x + 25] = 1;
	map[block_y - 1][block_x + 25] = 1;
	map[block_y + 0][block_x + 25] = 1;
	map[block_y + 1][block_x + 25] = 1;
	map[block_y + 2][block_x + 25] = 1;

	map[block_y - 14][block_x + 30] = 1; //장애물 5번
	map[block_y - 13][block_x + 30] = 1;
	map[block_y - 12][block_x + 30] = 1;
	map[block_y - 11][block_x + 30] = 1;
	map[block_y - 10][block_x + 30] = 1;
	map[block_y -  9][block_x + 30] = 1;
	textcolor(WHITE, BLACK);
}

void init_block2()
{
	textcolor(BLACK, WHITE);
	map[block_y - 2][block_x + 35] = 1;	//장애물 6번
	map[block_y - 3][block_x + 36] = 1;
	map[block_y - 4][block_x + 37] = 1;
	map[block_y - 5][block_x + 38] = 1;
	map[block_y - 6][block_x + 39] = 1;

	map[block_y - 11][block_x + 11] = 1; //장애물 7번
	map[block_y - 12][block_x + 12] = 1;
	map[block_y - 13][block_x + 13] = 1;
	map[block_y - 14][block_x + 14] = 1;
	map[block_y - 15][block_x + 15] = 1;

	map[block_y - 2][block_x - 15] = 1;
	map[block_y - 1][block_x - 14] = 1;
	map[block_y][block_x - 13] = 1;
	map[block_y + 1][block_x - 12] = 1;
	map[block_y][block_x - 11] = 1;
	map[block_y - 1][block_x - 10] = 1;
	map[block_y - 2][block_x - 9] = 1;

	map[block_y - 14][block_x - 29] = 1;
	map[block_y - 13][block_x - 30] = 1;
	map[block_y - 12][block_x - 31] = 1;
	map[block_y - 11][block_x - 32] = 1;

	map[block_y - 7][block_x +3] = 1;
	map[block_y - 6][block_x + 4] = 1;
	map[block_y - 5][block_x + 5] = 1;
	map[block_y - 4][block_x + 6] = 1;


	
	
	textcolor(WHITE, BLACK);
}

void show_block()
{
	int x, y;
	for (x = 0; x < WIDTH - 1; x++) {
		for (y = 0; y < HEIGHT; y++) {
			if (map[y][x] == 1) {
				gotoxy(x, y);
				printf("□"); // ㅁ 한자.
			}
		}
	}
}


void draw_box(int x1, int y1, int x2, int y2, char ch)
{
	int x, y;

	y = y1;

	for (x = x1; x <= x2; x++) {
		gotoxy(x, y);
		printf("%c", ch);
	}

	y = y2;

	for (x = x1; x <= x2; x++) {
		gotoxy(x, y);
		printf("%c", ch);
	}

	x = x2;

	for (y = y1; y <= y2; y++) {
		gotoxy(x, y);
		printf("%c", ch);
	}

	x = x1;

	for (y = y1; y <= y2; y++) {
		gotoxy(x, y);
		printf("%c", ch);
	}
}

void draw_box2(int x1, int y1, int x2, int y2, char *ch)
{
	int x, y;

	y = y1;

	for (x = x1; x <= x2; x += 2) {
		gotoxy(x, y);
		printf("%s", ch);
	}

	y = y2;

	for (x = x1; x <= x2; x += 2) {
		gotoxy(x, y);
		printf("%s", ch);
	}

	x = x2;

	for (y = y1; y <= y2; y++) {
		gotoxy(x, y);
		printf("%s", ch);
	}

	x = x1;

	for (y = y1; y <= y2; y++) {
		gotoxy(x, y);
		printf("%s", ch);
	}
}

void showscore(int a, int b, int c)
{
	textcolor(GREEN2, GRAY1);
	gotoxy(83, 5);
	printf("Score  : %d", a);
	gotoxy(83, 6);
	printf("Life   : %d  ", b);
	gotoxy(83, 7);
	printf("Length : %d ", c);
	textcolor(BLACK, BLACK);
}

#define GOLD	"★"
void show_gold()
{
	int x, y;
	x = rand() % WIDTH + 1;
	y = rand() % (HEIGHT - 1) + 1;  // 제일 상단은 피한다
	textcolor(WHITE, BLACK);
	gotoxy(x, y);
	printf(GOLD);
	golds[x][y] = 1;
	textcolor(BLACK, BLACK);
}

#define item	"◎"
void show_item()
{
	int x, y;
	x = rand() % WIDTH + 1;
	y = rand() % (HEIGHT - 1) + 1;  // 제일 상단은 피한다
	textcolor(YELLOW1, BLACK);
	gotoxy(x, y);
	printf(item);
	items[x][y] = 1;
	textcolor(BLACK, BLACK);
}


void stage2() {
	Delay = 90;
	goldinterval = 3;
	init_block1();
	show_block();
}

void stage3() {
	Delay = 100;
	goldinterval = 2;
	init_block2();
	show_block();
}

void stage4() {
	Delay = 80;	
}

void main()
{
	int i, x, y;
	char buf[100];
	int save = 3;
	int c1, c2;
	unsigned char last_ch, ch;
	int oldx, oldy, newx, newy;
	int run_time, start_time, item_time;
	int gold_time;

	removeCursor();
	cls(BLACK, WHITE);
	gotoxy(25, 11);
	printf("==========SNAKE GAME==========\n");
	gotoxy(28, 12);
	printf("Press any key to start");

	draw_box(0, 0, WIDTH, HEIGHT, '*'); // 화면에 box를 그린다.

	while (1) {
		for (i = 0; i <= 9; i++) {
			draw_box(1 + i, i + 1, WIDTH - i, HEIGHT - i, 'V');
			Sleep(70);
		}
		for (i = 9; i >= 0; i--) {
			draw_box(1 + i, i + 1, WIDTH - i, HEIGHT - i, ' ');
			Sleep(70);
		}
		if (kbhit())
			break;
	}
	getch();
START:
	gold_time = item_time = 0;
	start_time = time(NULL);
	
	gotoxy(21, 15); // 끝나는 화면 위치
	cls(BLACK, WHITE);
	draw_box2(0, 0, WIDTH, HEIGHT, "□");

	newx = oldx = 40;
	newy = oldy = 12;

	for (i = 0; i < SNAKE_LENGTH; i++) {
		snake[i].X = oldx - i * 2;
		snake[i].Y = oldy;
	}
	removeCursor();
	putstar(oldx, oldy, STAR);
	showsnake();
	keep_moving = 1;
	last_ch = RIGHT; // 일단 오른쪽으로 이동한다. 
	
	while (1) {
		showscore(score, save, snake_length);	// 화면에 점수, 생명, 길이 표시

		run_time = time(NULL) - start_time;
		if (run_time > gold_time && (run_time%goldinterval == 0)) {
			show_gold();
			gold_time = run_time; // 마지막 GOLD 표시 시간 기억
		}
		if (run_time > item_time && (run_time%iteminterval == 0)) {
			show_item();
			item_time = run_time; // 마지막 item 표시 시간 기억
		}
		

		if (map[newy][newx-1]) {		// 장애물에 부딪히면
			save = 0;					// 목숨 0
		}
		else if (map[newy][newx]) {		// 장애물에 부딪히면
			save = 0;					// 목숨 0
		}

		if (save == 0) {		// 생명을 다 사용하면
			while (1) {
				do {
					c1 = rand() % 16;
					c2 = rand() % 16;
				} while (c1 == c2);
				textcolor(c1, c2);
				gotoxy(32, 10);
				printf("* No Life... You Lose...*");
				Sleep(300);
			}
			break;
		}

		if (golds[newx][newy] || golds[newx-1][newy] ) {	// 골드를 먹으면
			score += 100;			// 점수 100점 획들
			golds[newx][newy] = 0;	// 먹은 골드는 화면에서 사라짐
			snake_length++;			// 길이가 한칸 늘어남
		}

		if (items[newx][newy] || items[newx - 1][newy] ) {	// 아이템을 먹으면
			snake_length--;			// 길이가 한칸 줄어듦 
		}

		if (score == 300) {		// 300점 달성하면
			stage2();			// 2단계
		}
		if (score >= 1000) {	// 1000점 달성하면
			stage3();			// 3단계
		}
		if (score >= 1500) {	// 1500점 달성하면
			stage4();			// 4단계
		}

		if (kbhit() == 1) {  // 키보드가 눌려져 있으면
			char c1;
			c1 = getch(); // key 값을 읽는다
			if (c1 == '\0') // VS 의 NULL 값은 Error이다.
				continue;
			else
				ch = c1;	 // NULL이 아니면 정상 입력이다.
			if (ch == ESC)	// ESC 누르면 프로그램 종료
				exit(0);
			if (ch == SPECIAL1) { 
				ch = getch();
				switch (ch) {
				case UP:
				case DOWN:
				case LEFT:
				case RIGHT:
					last_ch = ch;
					keep_moving = 1;
					break;
				default:
					keep_moving = 0;
				}
			}
		}
		if (keep_moving) {	// 움직이고 있으면

			switch (last_ch) {
			case UP:
				if (oldy > 1)
					newy = oldy - 1;
				else {
					if (snake_length < SNAKE_LENGTH_MAX) {
						save = 0;
					}
				}
				break;

			case DOWN:
				if (oldy < HEIGHT - 1)
					newy = oldy + 1;
				else {
					if (snake_length < SNAKE_LENGTH_MAX) {
						save = 0;
					}
				}
				break;

			case LEFT:
				if (oldx > 1)
					newx = oldx - 2;
				else {
					newx = WIDTH - 2;
					last_ch = RIGHT; newx = 1;
					if (snake_length < SNAKE_LENGTH_MAX) {
						score -= 100;
						save--;
					}
				}
				break;

			case RIGHT:
				if (oldx < WIDTH - 1)
					newx = oldx + 2;
				else {
					newx = 0;
					last_ch = LEFT; newx = WIDTH - 1;
					if (snake_length < SNAKE_LENGTH_MAX) {
						score -= 100;
						save--;
					}
					break;
				}
			}
			erasestar(oldx, oldy); // 마지막 위치의 * 를 지우고
			putstar(newx, newy, STAR); // 새로운 위치에서 * 를 표시한다.
			movesnake(newx, newy);
			oldx = newx; // 마지막 위치를 기억한다.
			oldy = newy;
			keep_moving = 1; //1:계속이동, 0:한번에 한칸씩이동
		}
		Sleep(Delay); // Delay를 줄이면 속도가 빨라진다.
		
		if (score >= 2000) {	// 점수 2000점을 달성하면 문구가 뜨면서 종료 
			while (1) {
				do { 
					c1 = rand() % 16;
					c2 = rand() % 16;
				} while (c1 == c2);
				textcolor(c1, c2);
				gotoxy(32, 10);
				printf("** You are the winner **");
				Sleep(300);
			}
			break;
		}
	}
}