#ifndef __DBERGV_STACK__
#define __DBERGV_STACK__
#include <stdlib.h>
#include "value.h"

typedef struct node_s {
	value_t val;
	struct node_s * next;
} node_t;

typedef struct {
	node_t * head;
	size_t num_elements;
} Stack;

Stack * Stack_create();

size_t Stack_size(Stack * stk);

void Stack_push(Stack * stk, value_t val);

value_t Stack_pop(Stack * stk);

void Stack_destroy(Stack * stk);


#endif /* __DBERGV_STACK__ */