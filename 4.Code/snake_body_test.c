#include <stdio.h>
#include <stdlib.h>


void main(){

	typedef struct node * NodePtr;
	
	typedef struct node{
		int col;
		int row;
		cellptr next;
	} Node;

	typedef struct snake{
		NodePtr first;
		NodePtr last;
		int lenth;	
	} Snake;

	Header * header;

	NodePtr create(int col, int row){
		NodePtr temp;
		temp = (NodePtr)malloc(sizeof(Node));
		temp->col = col;
		temp->row = row;
		temp->next = NULL;
		return temp
	}

	void insert(int col, int row, Snake snake){
		NodePtr newLast, originLast;
		newLast = create(col, row);
		newLast->next = originLast;
		snake.last = newLast;
	}

	void initSnake(){
		
	}



}
