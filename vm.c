#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "stack.h"
#include "value.h"
#include "mem.h"
#include "vm.h"
#include "exec.h"

// do register lookups, if necessary
value_t eval(vm_t * vm, value_t v){
	if(v > MAX_VAL) return vm->regs[Value_get_register_idx(&v)];
	else return v;
}

void load_binary(vm_t * vm){
	assert(vm);
	assert(vm->mem);

	FILE * fp;
	fp = fopen(INPUT_FILENAME,"rb");
	value_t buffer;

	while(fread(&buffer,sizeof(value_t),1,fp) > 0){
		//printf("%" PRIu16 " \n", buffer);
		cell c = {((uint16_t) Memory_size(vm->mem)),buffer};

		Memory_append(vm->mem,&c);
	}
    	
    fclose(fp);
}


// write VM state to fp (stdout by default)
void state_dump(vm_t * vm, FILE * fp){
	assert(vm);

	if(!fp)
		fp = stdout;

	printf("Current VM State: \n----------\n");

	printf("Registers:\n");
	for(int i = 0; i < NUM_REGS; i++){
		printf("\tr%d -> %" PRIu16 "\n",i,vm->regs[i]);
	}

	size_t stk_size = Stack_size(vm->stk);
	printf("\nStack (size %zu):\n",stk_size);
	Stack * tmp = Stack_create();
	for(int i = 0; i < stk_size; i++){
		value_t v = Stack_pop(vm->stk);
		printf("\ts%d -> %" PRIu16 "\n",i,v);
		Stack_push(tmp,v);
	}
	for(int i = 0; i < stk_size; i++){
		value_t v = Stack_pop(tmp);
		Stack_push(vm->stk,v);
	}
	assert(Stack_size(tmp) == 0);
	Stack_destroy(tmp);

	printf("\nProgram Counter -> %zu\n",vm->pc);
}

// same as above, but also prints memory
void state_dump_full(vm_t * vm,FILE * fp){
	state_dump(vm,fp);

	if(!fp)
		fp = stdout;

	size_t idx = 0;
	const cell * c = Memory_get(vm->mem,idx);

	printf("\nMemory (size %zu):\n",Memory_size(vm->mem));
	while(c){
		printf("\tm%" PRIu16 " -> %" PRIu16 "\n",c->addr,c->value);
		c = Memory_get(vm->mem,++idx);
	}
}


vm_t * init_vm(){
	vm_t * vm = malloc(sizeof(vm_t));

	vm->regs = calloc(NUM_REGS,sizeof(value_t));
	vm->stk = Stack_create();
	vm->mem = Memory_create();
	vm->pc = 0;

	vm->stdin_buf = NULL;
	vm->stdin_idx = 1;
	vm->stdin_buf_size = 0;
	vm->stdin_str_len = 0;

	vm->ff_flag = 0;
	vm->ff_script = NULL;

	load_binary(vm);

	return vm;
}

void shutdown_vm(vm_t * vm){
	assert(vm);
	
	if(vm->regs) free(vm->regs);
	if(vm->stk) Stack_destroy(vm->stk);
	Memory_destroy(vm->mem);
	if(vm->stdin_buf) free(vm->stdin_buf);
	if(vm->ff_script) fclose(vm->ff_script);

	free(vm);
}

int main(){
	vm_t * vm = init_vm();

	execute(vm,LOG_TRACE);

	shutdown_vm(vm);
	return 0;
}