/* snake_ver1.c
* author: 2013019033_수학과_정인준
* description
* - moving_add_snake.c에 item 설정 및 방향 제한, 게임 종료 조건 추가
* - compile: cc snake_ver1.c set_ticker.c -lcurses -o snake_ver1
*/

#include <unistd.h>
#include <stdio.h>
#include <curses.h>
#include <ncurses.h>
#include <stdlib.h>

#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <time.h>

#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4

char HEAD = '$';			/* the head of snake	    */
char BODY = '*';			/* the body of snake	    */
char BLANK = ' ';
char ITEM = '!';
int dir = RIGHT;			/* where head is going to   */
int predir = RIGHT;			/* where head was from 	    */
int done = 0;
int delay = 100;			/* how long to wait	    */
float score = 0.0;
char * score_buf;
int decimal, sign;
char * lenth_buf;

/* the position of item */
int XPOS;				
int YPOS;

void set_cr_noecho_mode(void);
int set_ticker(int);
bool checkPass(int y, int x);
void changeDir(int dir);


typedef struct node * NodePtr;

typedef struct node {			/* the cell of snake	    */
	int row;
	int col;
	NodePtr next;
	NodePtr back;
} Node;

typedef struct snake * SnakePtr;

typedef struct snake {			/* a snake		    */
	NodePtr first;
	NodePtr last;
	int lenth;	
} Snake;

NodePtr create(int row, int col) {
	NodePtr temp;
	temp = (NodePtr)malloc(sizeof(Node));
	temp->row = row;
	temp->col = col;
	temp->next = NULL;
	temp->back = NULL;
	return temp;
}

void insertHeader(int row, int col, SnakePtr s) {
	NodePtr newHeader, preHeader;
	newHeader = create(row, col);
	preHeader = s->first;
	preHeader->next = newHeader;
	newHeader->back = preHeader;
	s->first = newHeader;
}

SnakePtr s;

void initSnake(){
	s = (SnakePtr)malloc(sizeof(Snake));
	NodePtr rear;
	rear = create(10, 0);
	s->first = rear;
	s->last = rear;
	s->lenth = 4;

	insertHeader(10, 1, s);
	insertHeader(10, 2, s);
	insertHeader(10, 3, s);
}

void setWindow(){
	move(2, 10);						
	addstr("Score: ");

	move(2, 30);						
	addstr("Lenth: ");

}

void moving(){
	
	void on_alarm(int);		/* handler for alarm 	    */
	void on_input(int);		/* handler for keybd 	    */
	void enable_kbd_signals();

	initscr();			/* set up screen 	    */

	set_cr_noecho_mode();
	clear();

	signal(SIGIO, on_input);	/* install a handler        */
	enable_kbd_signals();           /* turn on kbd signals      */
	signal(SIGALRM, on_alarm);	/* install alarm handler    */
	set_ticker(delay);		/* start ticking     	    */

	setWindow();
	/* draw initial image       */
	NodePtr temp = s->last;
	for (int i = 1; i < s->lenth; i++){
		move(temp->row, temp->col);						
		addch(BODY);	
		temp = temp->next;
	}

	mvaddch(s->first->row, s->first->col, HEAD);	

	srand((unsigned)time(NULL));
	XPOS = (rand() % 10) + 5;
	YPOS = (rand() % 10) + 8;
	mvaddch(YPOS, XPOS, ITEM);


	while(!done)			/* the main loop	    */
		pause();

	endwin();
}


/* change the direction of HeadOfSnake */
void on_input(int signum){
	int c = getchar();	/* grep the char */

	if (c == 'Q' || c == EOF)	/* end */
		done = 1;

	else if ( c == 'W' )
		dir = UP;
	else if ( c == 'A' )
		dir = LEFT;
	else if ( c == 'S' )
		dir = DOWN;
	else if ( c == 'D' )
		dir = RIGHT;
}

void updateWindow(){
	/* plus score and lenth */
	score += 5;
	score_buf = fcvt(score, 4, &decimal, &sign);
	if (score < 10)
		mvaddch(2, 17, score_buf[0]);
	else if (score < 100){
		mvaddch(2, 17, score_buf[0]);
		mvaddch(2, 18, score_buf[1]);
	}
	else if (score < 1000){
		mvaddch(2, 17, score_buf[0]);
		mvaddch(2, 18, score_buf[1]);
		mvaddch(2, 19, score_buf[2]);
	}
	else if (score < 10000){
		mvaddch(2, 17, score_buf[0]);
		mvaddch(2, 18, score_buf[1]);
		mvaddch(2, 19, score_buf[2]);
		mvaddch(2, 20, score_buf[3]);
	}
	lenth_buf = fcvt((float) s->lenth, 1, &decimal, &sign);
	if (s->lenth < 10)
		mvaddch(2, 36, lenth_buf[0]);
	else {
		mvaddch(2, 36, lenth_buf[0]);
		mvaddch(2, 37, lenth_buf[1]);
	}

}

void deleteLast(){
	mvaddch(s->last->row, s->last->col, BLANK);		/* delete the tail of snake */
	s->last = s->last->next;				/* change the tail of snake */
}

void makeItem(){
	if ( ( (int) score ) % 200 == 0 ){
		mvaddch(YPOS, XPOS, BLANK);
		while(1){
			srand((unsigned)time(NULL));
			XPOS = (rand() % 10) + 5;
			YPOS = (rand() % 10) + 8;
			if (!checkPass(YPOS, XPOS))
				break;
		}
		mvaddch(YPOS, XPOS, ITEM);
	}
}

bool checkPass(int y, int x){
	NodePtr temp = s->last;
	for(int i = 1; i < s->lenth; i++){
		if(temp->row == y && temp->col == x)
			return true;
		temp = temp->next;
	}
	return false;
}

void changeDir(int dir){

	switch(dir){
		case LEFT:
			if(s->first->row == s->first->back->row && ((s->first->col) - 1) == s->first->back->col)
				dir = predir;
			break;

		case RIGHT:
			if(s->first->row == s->first->back->row && ((s->first->col) + 1) == s->first->back->col)
				dir = predir;
			break;

		case UP:
			if(((s->first->row) - 1) == s->first->back->row && s->first->col == s->first->back->col)
				dir = predir;
			break;

		case DOWN:
			if(((s->first->row) + 1) == s->first->back->row && s->first->col == s->first->back->col)
				dir = predir;
			break;
	}


	switch(dir){		
		case LEFT:			
			predir = dir;
			insertHeader(s->first->row, (s->first->col) - 1, s);
			mvaddch(s->first->row, s->first->col, HEAD);
			mvaddch(s->first->back->row, s->first->back->col, BODY);
			if (checkPass(s->first->row, s->first->col))
				getch();	
		
			if (s->first->row == YPOS && s->first->col == XPOS)
				s->lenth += 1;
			else
				deleteLast();
			refresh();
			break;
		case RIGHT:
			predir = dir;
			insertHeader(s->first->row, (s->first->col) + 1, s);
			mvaddch(s->first->row, s->first->col, HEAD);
			mvaddch(s->first->back->row, s->first->back->col, BODY);
			if (checkPass(s->first->row, s->first->col))
				getch();			

			if (s->first->row == YPOS && s->first->col == XPOS)
				s->lenth += 1;
			else
				deleteLast();
			refresh();
			break;
		case UP:
			predir = dir;
			insertHeader((s->first->row) - 1, s->first->col, s);
			mvaddch(s->first->row, s->first->col, HEAD);
			mvaddch(s->first->back->row, s->first->back->col, BODY);
			if (checkPass(s->first->row, s->first->col))
				getch();			

			if (s->first->row == YPOS && s->first->col == XPOS)
				s->lenth += 1;
			else
				deleteLast();
			refresh();
			break;
		case DOWN:
			predir = dir;
			insertHeader((s->first->row) + 1, s->first->col, s);
			mvaddch(s->first->row, s->first->col, HEAD);
			mvaddch(s->first->back->row, s->first->back->col, BODY);
			if (checkPass(s->first->row, s->first->col))
				getch();			

			if (s->first->row == YPOS && s->first->col == XPOS)
				s->lenth += 1;
			else
				deleteLast();
			refresh();
			break;
	}
	
	// 임시로 설정한 가이드 라인 수정 필요
	if (dir == LEFT && s->first->col <= 0)
		dir = RIGHT;
	else if (dir == RIGHT && s->first->col >= COLS)
		dir = LEFT;
	else if (dir == UP && s->first->row <= 0)
		dir = DOWN;
	else if (dir == DOWN && s->first->row >= 100)
		dir = UP;
}

/* change the position of HeadOfSnake */
void on_alarm(int signum){

	if (s->lenth > 20)
		delay == 40;

	else if (s->lenth > 15)
		delay == 60;

	else if (s->lenth > 12)
		delay == 80;

	updateWindow();
	makeItem();
//	signal(SIGALRM, on_alarm);
	
	changeDir(dir);


}

void enable_kbd_signals()
{
	int  fd_flags;

	fcntl(0, F_SETOWN, getpid());         /*set io signal to current pid*/
	fd_flags = fcntl(0, F_GETFL);         /*get status of the file*/
	fcntl(0, F_SETFL, (fd_flags|O_ASYNC));/*set status to tty with O_ASYNC*/
}

void set_cr_noecho_mode(void){
	struct termios ttystate;

	tcgetattr(0, &ttystate);
	ttystate.c_lflag &= ~ICANON;
	ttystate.c_lflag &= ~ECHO;
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(0, TCSANOW, &ttystate);
}

void main(){
	initSnake();	
	moving();
}


















