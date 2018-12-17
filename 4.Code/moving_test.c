/* moving_test.c
* author: 2013019033_수학과_정인준
* description
* - ch7의 bounce_aio.c를 활용하여 입력에 따른 뱀의 방향 전환을 구현하는 것이 목표
* - compile: cc moving_test.c set_ticker.c -lcurses -o moving_test
*/

#include <unistd.h>
#include <stdio.h>
#include <curses.h>
#include <ncurses.h>

#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#define UP 1
#define LEFT 2
#define DOWN 3
#define RIGHT 4

char PLAYER = '@';
char BLANK = ' ';
int row = 10;				/* current row		    */
int col = 0;				/* current column 	    */
int dir = RIGHT;			/* where we are going 	    */
int delay = 200;			/* how long to wait	    */
int done = 0;

void set_cr_noecho_mode(void);
int set_ticker(int);

void moving(){
	
	void on_alarm(int);		/* handler for alarm 	    */
	void on_input(int);		/* handler for keybd 	    */
	void	enable_kbd_signals();

	initscr();			/* set up screen 	    */

	set_cr_noecho_mode();
	clear();

	signal(SIGIO, on_input);	/* install a handler        */
	enable_kbd_signals();           /* turn on kbd signals      */
	signal(SIGALRM, on_alarm);	/* install alarm handler    */
	set_ticker(delay);		/* start ticking     	    */

	move(row, col);			/* get into position        */
	addch(PLAYER);			/* draw initial image       */

	while(!done)			/* the main loop	    */
		pause();

	endwin();
}

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

void on_alarm(int signum){
	signal(SIGALRM, on_alarm);
	mvaddch(row, col, BLANK);

	switch(dir){		/* change the position of PLAYER */
		case LEFT:
			col -= 1;
			mvaddch(row, col, PLAYER);
			refresh();
			break;
		case RIGHT:
			col += 1;
			mvaddch(row, col, PLAYER);
			refresh();
			break;
		case UP:
			row -= 1;
			mvaddch(row, col, PLAYER);
			refresh();
			break;
		case DOWN:
			row += 1;
			mvaddch(row, col, PLAYER);
			refresh();
			break;
	}
	
	// 임시로 설정한 가이드 라인
	if (dir == LEFT && col <= 0)
		dir = RIGHT;
	else if (dir == RIGHT && col >= COLS)
		dir = LEFT;
	else if (dir == UP && row <= 0)
		dir = DOWN;
	else if (dir == DOWN && row >= 100)
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
	moving();
}


















