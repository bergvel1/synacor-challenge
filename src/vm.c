#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include "../include/vm.h"
#include "../include/exec.h"
#include "../include/debug.h"
#include "../include/logging.h"

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
		//fprintf(vm->out_fp,"%" PRIu16 " \n", buffer);
		cell c = {((uint16_t) Memory_size(vm->mem)),buffer};

		Memory_append(vm->mem,&c);
	}
    	
    fclose(fp);
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

	vm->out_fd = -1;
	vm->in_fd = -1;

	vm->out_fp = stdout;
	vm->in_fp = stdin;

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


int main(int argc, char* argv[]){
	vm_t * vm = init_vm();

	if((argc > 1) && (strcmp(argv[1],"-d") == 0))
		debugger(vm);

	else execute(vm,LOG_TRACE);

	shutdown_vm(vm);
	return 0;
}
