#include "../include/stack.h"
#include "../include/value.h"
#include <assert.h>


Stack * Stack_create(){
	Stack * stk = (Stack *) malloc(sizeof(Stack));
	stk->head = NULL;
	stk->num_elements = 0;

	return stk;
}

size_t Stack_size(Stack * stk){
	assert(stk);

	return stk->num_elements;
}

void Stack_push(Stack * stk, value_t val){
	assert(stk);

	node_t * n = (node_t *) malloc(sizeof(node_t));
	assert(n);

	n->val = val;
	n->next = stk->head;
	stk->head = n;
	stk->num_elements++;
}

value_t Stack_pop(Stack * stk){
	assert(stk);
	assert(stk->num_elements > 0);

	value_t ret = stk->head->val;
	node_t * next_head = stk->head->next;
	free(stk->head);
	stk->head = next_head;
	stk->num_elements--;

	return ret;
}

value_t Stack_peek(Stack * stk, int idx){
	assert(stk);
	assert(stk->num_elements > idx);

	int curr_idx = 0;
	node_t * itr = stk->head;

	while(itr){
		if(curr_idx == idx)
			return itr->val;

		itr = itr->next;
		curr_idx++;
	}

	return -1; // should not be reached
}

void Stack_destroy(Stack * stk){
	assert(stk);

	node_t * it1 = stk->head;
	node_t * it2 = stk->head;

	while(it2){
		it2 = it1->next;

		free(it1);
		it1 = it2;
	}

	free(stk);
}