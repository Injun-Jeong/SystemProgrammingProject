/* moving_add_snake.c
* author: 2013019033_수학과_정인준
* description
* - moving_test.c에 snake 구조체를 적용시키기
* - compile: cc moving_add_snake.c set_ticker.c -lcurses -o moving_add_snake
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

#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4

char HEAD = '$';			/* the head of snake	    */
char BODY = '*';			/* the body of snake	    */
char BLANK = ' ';
int dir = RIGHT;			/* where head is going 	    */
int done = 0;
int delay = 200;			/* how long to wait	    */
int score = 0;

/* the position of item */
int XPOS;				
int YPOS;

void set_cr_noecho_mode(void);
int set_ticker(int);

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
	addstr("Score");
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

	move(s->first->row, s->first->col);						
	addch(HEAD);	


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


/* change the position of HeadOfSnake */
void on_alarm(int signum){
	signal(SIGALRM, on_alarm);
	mvaddch(s->last->row, s->last->col, BLANK);		/* delete the tail of snake */
	s->last = s->last->next;				/* change the tail of snake */

	switch(dir){		
		case LEFT:
			insertHeader(s->first->row, (s->first->col) - 1, s);
			mvaddch(s->first->row, s->first->col, HEAD);
			mvaddch(s->first->back->row, s->first->back->col, BODY);
			refresh();
			break;
		case RIGHT:
			insertHeader(s->first->row, (s->first->col) + 1, s);
			mvaddch(s->first->row, s->first->col, HEAD);
			mvaddch(s->first->back->row, s->first->back->col, BODY);
			refresh();
			break;
		case UP:
			insertHeader((s->first->row) - 1, s->first->col, s);
			mvaddch(s->first->row, s->first->col, HEAD);
			mvaddch(s->first->back->row, s->first->back->col, BODY);
			refresh();
			break;
		case DOWN:
			insertHeader((s->first->row) + 1, s->first->col, s);
			mvaddch(s->first->row, s->first->col, HEAD);
			mvaddch(s->first->back->row, s->first->back->col, BODY);
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


















