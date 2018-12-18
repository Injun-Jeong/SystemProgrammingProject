/* title_test.c
* author: 2013019033_수학과_정인준
* description
* - 프로그램을 실행 시, 사용자로부터 입력을 받을 때까지 기다리는 첫 화면을 구현하는 것이 목표
* - compile: cc title_test.c -lcurses -o title_test
*/
#include <stdio.h>
#include <curses.h>

void title(){
	char key;

	initscr();
//	set_cr_noecho_mode();
//	updateWindow();

	
	clear();
	mvaddstr(10, 20, "title");
	
	refresh();
	key = getch();	// (사용자로 부터 입력을 받을 때 까지) 프로그램을 잠시 일시정지 시키기 위함
	if (key == 'Y'){
		mvaddstr(11, 20, "press Y");
		refresh();	
	}
	else if (key == 'N'){
		mvaddstr(11, 20, "press N");	
		refresh();	
	}

	endwin();
}

void main(){
	title();
}
