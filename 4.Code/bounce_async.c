#include	<stdio.h>
#include	<curses.h>
#include	<signal.h>
#include	<fcntl.h>
#include 	<string.h>

#include	<unistd.h>
#include	<termio.h> 

#define	MESSAGE	"@"
#define	BLANK	" "

int	row   = 10;	/* current row		*/
int	col   =  0;	/* current column	*/
int	dir   =  1;	/* where we are going	*/
int	delay = 200;	/* how long to wait	*/
int	done  = 0;

void set_cr_noecho_mode(void); 
int set_ticker(int); 		

int main(void)
{	void	on_alarm(int);	/* handler for alarm	*/
	void	on_input(int);	/* handler for keybd    */
	void	enable_kbd_signals();

	initscr();		/* set up screen */
	
	set_cr_noecho_mode(); 

	clear();

	signal(SIGIO, on_input);          /* install a handler        */
	enable_kbd_signals();             /* turn on kbd signals      */
	signal(SIGALRM, on_alarm);        /* install alarm handler    */
	set_ticker(delay);		  /* start ticking     */

	move(row,col);		          /* get into position      */
	addstr( MESSAGE );	          /* draw initial image       */

	while( !done )			  /* the main loop */
		pause();
	
	endwin();

	return 0;
}

void on_input(int signum)
{		
	int 	c = getchar();		/* grab the char */

	if ( c == 'Q' || c == EOF )
		done = 1;
	else if ( c == ' ' )
		dir = -dir;
	else if ( c == 'D' )
		dir = -dir;
/*
	switch(c) {
		case KEY_LEFT:
			dir = -dir;
			break;
	}
*/
}


void on_alarm(int signum)
{
	signal(SIGALRM, on_alarm);	/* reset, just in case	*/
	mvaddstr( row, col, BLANK );	/* note mvaddstr()	*/
	col += dir;			/* move to new column	*/
	mvaddstr( row, col, MESSAGE );	/* redo message		*/
	refresh();			/* and show it		*/

	/*
	 * now handle borders
	 */
	if ( dir == -1 && col <= 0 )
		dir = 1;
	else if ( dir == 1 && col+strlen(MESSAGE) >= COLS )
		dir = -1;
}

/*
 * install a handler, tell kernel who to notify on input, enable signals
 */
void enable_kbd_signals()
{
	int  fd_flags;

	fcntl(0, F_SETOWN, getpid());         /*set io signal to current pid*/
	fd_flags = fcntl(0, F_GETFL);         /*get status of the file*/
	fcntl(0, F_SETFL, (fd_flags|O_ASYNC));/*set status to tty with O_ASYNC*/
}

void set_cr_noecho_mode(void)
{
	struct termios ttystate;
	
	tcgetattr( 0, &ttystate );
	ttystate.c_lflag	&= ~ICANON;
	ttystate.c_lflag	&= ~ECHO;
	ttystate.c_cc[VMIN]	= 1;
	tcsetattr( 0, TCSANOW, &ttystate);
}


