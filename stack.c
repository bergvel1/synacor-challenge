#include "stack.h"
#include "value.h"
#include <assert.h>


Stack * Stack_create(){
	Stack * stk = malloc(sizeof(Stack));
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

	node_t * n = malloc(sizeof(node_t));
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