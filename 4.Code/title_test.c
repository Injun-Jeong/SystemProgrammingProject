/* title_test.c
* author: 2013019033_수학과_정인준
* description
* - 프로그램을 실행 시, 사용자로부터 입력을 받을 때까지 기다리는 첫 화면을 구현하는 것이 목표
*/
#include <stdio.h>
#include <curses.h>

void title(){
	initscr();
	
	clear();
	move(10, 20);
	addstr("title");
	move(LINES-1, 0);
	
	refresh();
	getch();	// (사용자로 부터 입력을 받을 때 까지) 프로그램을 잠시 일시정지 시키기 위함
	
	endwin();
}

void main(){
	title();
}
