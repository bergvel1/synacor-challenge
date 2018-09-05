#include <stdio.h>
#include "../include/stack.h"

Stack * stk = NULL;

int r0 = 4;
int r1 = 1;
int r7 = 1;

void f6027(){
	F6027: ;
	if(r0){
		goto F6035;
	}

	r0 = r1 + 1;
	return;
}

void f6035(){
	F6035: ;
	if(r1){
		goto F6048;
	}

	r0--;
	r1 = r7;

	f6027();
	return;
}

void f6048(){
	F6048: ;
	Stack_push(stk,(value_t r0));
	r1--;

	f6027();
	r1 = r0;
	r0 = Stack_pop(stk);
	r0--;
	f6027();
	return;
}

int main(){
	stk = Stack_create();

	// PC = 5483
	r0 = 4;
	r1 = 1;
	f6027();
	printf("r0 == %d\n",r0);

	Stack_destroy(stk);
	return 0;
}