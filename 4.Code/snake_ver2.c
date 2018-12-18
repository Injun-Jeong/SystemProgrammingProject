/* snake_ver2.c
* author: 2013019033_수학과_정인준
* description
* - snake_ver1.c에 시작 화면 추가 및 UI 정리
* - compile: cc snake_ver2.c set_ticker.c -lcurses -o snake_ver2
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
char OBSTACLE = 'X';
int dir = RIGHT;			/* where head is going to   */
int predir = RIGHT;			/* where head was from 	    */
int done = 0;
int delay = 70;				/* how long to wait	    */
float score = 0.0;
char * score_buf;
int decimal, sign;
char * lenth_buf;
int expectation = 100;

/* the position of item */
int XPOS;				
int YPOS;

/* the position of obstacle */
int XPOS_O;				
int YPOS_O;

void set_cr_noecho_mode(void);
int set_ticker(int);
bool checkPass(int y, int x);
void changeDir(int dir);
void checkEnd();
void home();
void helpWindow();

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
	rear = create(11, 23);
	s->first = rear;
	s->last = rear;
	s->lenth = 6;

	insertHeader(11, 24, s);
	insertHeader(12, 24, s);
	insertHeader(13, 24, s);
	insertHeader(13, 25, s);
	insertHeader(13, 26, s);

}

void setWindow(){
	mvaddstr(2, 30, "[ The snake game ]");
	mvaddstr(4, 15, "================================================");
	mvaddstr(5, 25, "Score: ");
	mvaddstr(5, 45, "Lenth: ");
	mvaddstr(6, 15, "================================================");
	mvaddstr(7, 15, "[][][][][][][][][][][][][][][][][][][][][][][][]");
	for (int i = 7; i < 30; i++){
		mvaddstr(i, 13, "[]");
		mvaddstr(i, 63, "[]");
	} 
	mvaddstr(29, 15, "[][][][][][][][][][][][][][][][][][][][][][][][]");
	mvaddstr(30, 15, "================================================");
	mvaddstr(31, 18, "Quit: Q"); mvaddstr(31, 34, "!: Item");
	mvaddstr(31, 50, "X: Obstacle");
	mvaddstr(32, 15, "================================================");
}

bool line(int y, int x){
	for (int i = 7; i <= 29; i++){
		if (y == i && (x == 14 || x == 63))
			return true; 
	}
	for (int j = 15; j <= 62; j++){
		if (x == j && (y == 7 || y == 29))
			return true;
	}	
	return false;
}

void title(){
	char key;
	initscr();
	clear();
	set_cr_noecho_mode();
	setWindow();
	mvaddstr(14, 22, "[ Hello! This is the snake game. ]");
	mvaddstr(17, 24, "Press 'Y' if you want to start.");	
	mvaddstr(18, 24, "Press 'N' if not.");
	mvaddstr(21, 22, "Press 'H' if you want to be helped.");
	refresh();
	key = getch();
	if (key == 'Y' || key == 'y'){
		initSnake(); home();
	}
	else if (key == 'H' || key == 'h')
		helpWindow();
	endwin();
}

void helpWindow(){
	char key;
	clear();
	setWindow();
	mvaddstr(14, 33, "[ Welcome! ]");
	mvaddstr(17, 22, "Press 'W' if you want going to UP.");	
	mvaddstr(18, 22, "Press 'A' if you want going to LEFT.");	
	mvaddstr(19, 22, "Press 'S' if you want going to DOWN.");	
	mvaddstr(20, 22, "Press 'D' if you want going to RIGHT.");	
	mvaddstr(22, 22, "Press 'Y' if you want to start.");
	refresh();
	key = getch();
	if (key == 'Y' || key == 'y'){
		initSnake(); home();
	}
}

void home(){
	void on_alarm(int);		/* handler for alarm 	    */
	void on_input(int);		/* handler for keybd 	    */
	void enable_kbd_signals();
	clear();
	signal(SIGIO, on_input);	/* install a handler        */
	enable_kbd_signals();           /* turn on kbd signals      */
	signal(SIGALRM, on_alarm);	/* install alarm handler    */
	set_ticker(delay);		/* start ticking     	    */
	setWindow();

	/* draw initial image       */
	NodePtr temp = s->last;
	for (int i = 1; i < s->lenth; i++){
		mvaddch(temp->row, temp->col, BODY);	
		temp = temp->next;
	}
	mvaddch(s->first->row, s->first->col, HEAD);	

	srand((unsigned)time(NULL));
	XPOS = (rand() % 47) + 15;
	YPOS = (rand() % 22) + 7;
	mvaddch(YPOS, XPOS, ITEM);

	srand((unsigned)time(NULL));
	XPOS_O = (rand() % 100)/3 + 15;
	YPOS_O = (rand() % 50)/3 + 7;
	mvaddch(YPOS_O, XPOS_O, OBSTACLE);

	while(!done)			/* the main loop	    */
		pause();
	endwin();
}


/* change the direction of HeadOfSnake */
void on_input(int signum){
	int c = getchar();	/* grep the char */
	if (c == 'Q' || c == EOF || c == 'q' )	/* end */
		done = 1;
	else if ( c == 'W' || c == 'w' )
		dir = UP;
	else if ( c == 'A' || c == 'a' )
		dir = LEFT;
	else if ( c == 'S' || c == 's' )
		dir = DOWN;
	else if ( c == 'D' || c == 'd' )
		dir = RIGHT;
}

void updateWindow(){
	mvaddch(5, 32, ' '); mvaddch(5, 33, ' ');
	mvaddch(5, 34, ' '); mvaddch(5, 35, ' ');	/* plus score and lenth */
	score += 5;
	score_buf = fcvt(score, 4, &decimal, &sign);
	if (score < 10)
		mvaddch(5, 32, score_buf[0]);
	else if (score < 100){
		mvaddch(5, 32, score_buf[0]); mvaddch(5, 33, score_buf[1]);
	}
	else if (score < 1000){
		mvaddch(5, 32, score_buf[0]); mvaddch(5, 33, score_buf[1]);
		mvaddch(5, 34, score_buf[2]);
	}
	else if (score < 10000){
		mvaddch(5, 32, score_buf[0]); mvaddch(5, 33, score_buf[1]);
		mvaddch(5, 34, score_buf[2]); mvaddch(5, 35, score_buf[3]);
	}
	lenth_buf = fcvt((float) s->lenth, 1, &decimal, &sign);
	if (s->lenth < 10)
		mvaddch(5, 51, lenth_buf[0]);
	else {
		mvaddch(5, 51, lenth_buf[0]); mvaddch(5, 52, lenth_buf[1]);
	}

}

void deleteLast(){
	mvaddch(s->last->row, s->last->col, BLANK);		/* delete the tail of snake */
	s->last = s->last->next;				/* change the tail of snake */
}

void makeItem(){
	if ( ( (int) score ) % 300 == 0 ){
		mvaddch(YPOS, XPOS, BLANK);
		while(1){
			srand((unsigned)time(NULL));
			XPOS = (rand() % 47) + 15;
			YPOS = (rand() % 21) + 8;
			if (!checkPass(YPOS, XPOS))
				break;
		}
		mvaddch(YPOS, XPOS, ITEM);
	}
}


void makeObstacle(){
	if ( ( (int) score ) % 100 == 0 ){
		mvaddch(YPOS_O, XPOS_O, BLANK);
		while(1){
			srand((unsigned)time(NULL));
			XPOS_O = (rand() % 100)/3 + 15;
			YPOS_O = (rand() % 50)/3 + 8;
			if (!checkPass(YPOS_O, XPOS_O))
				break;
		}
		mvaddch(YPOS_O, XPOS_O, OBSTACLE);
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

void checkEnd(){
	clear();	
	setWindow();
	updateWindow();
	mvaddstr(17, 34, "Game over.");
	mvaddstr(19, 28, "Press any key to quit.");
	refresh();
	pause();
	done = 1;
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
				checkEnd();
			if (s->first->row == YPOS_O && s->first->col == XPOS_O)
				checkEnd();
			if (line(s->first->row, s->first->col))
				checkEnd();
			if (s->first->row == YPOS && s->first->col == XPOS)
				score += 50;
			if (score >= expectation){
				s->lenth += 1;
				expectation += 100;
			}
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
				checkEnd();
			if (s->first->row == YPOS_O && s->first->col == XPOS_O)
				checkEnd();
			if (line(s->first->row, s->first->col))
				checkEnd();
			if (s->first->row == YPOS && s->first->col == XPOS)
				score += 50;
			if (score >= expectation){
				s->lenth += 1;
				expectation += 100;
			}
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
				checkEnd();
			if (s->first->row == YPOS_O && s->first->col == XPOS_O)
				checkEnd();
			if (line(s->first->row, s->first->col))
				checkEnd();
			if (s->first->row == YPOS && s->first->col == XPOS)
				score += 50;
			if (score >= expectation){
				s->lenth += 1;
				expectation += 100;
			}
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
				checkEnd();
			if (s->first->row == YPOS_O && s->first->col == XPOS_O)
				checkEnd();
			if (line(s->first->row, s->first->col))
				checkEnd();
			if (s->first->row == YPOS && s->first->col == XPOS)
				score += 50;
			if (score >= expectation){
				s->lenth += 1;
				expectation += 100;
			}
			else
				deleteLast();
			refresh();
			break;
	}
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
	makeObstacle();
	makeItem();
	signal(SIGALRM, on_alarm);
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
	title();	
}
