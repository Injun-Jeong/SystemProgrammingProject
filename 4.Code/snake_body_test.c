#include <stdio.h>
#include <stdlib.h>

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





void main(){

}
