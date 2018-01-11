#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "stack.h"
#include "value.h"
#include "mem.h"
#include "vm.h"
#include "exec.h"
#include "debug.h"

// do register lookups, if necessary
value_t eval(vm_t * vm, value_t v){
	if(v > MAX_VAL) return vm->regs[Value_get_register_idx(&v)];
	else return v;
}

// gives string representation of memory cells (e.g. turns opcodes into legible instructions)
// assumes buf contains adequate space to store characters
// updates pc_ptr to the address immediately following the last argument for the given instruction
char * string_of_cell(vm_t * vm, const cell * inst_ptr, char * buf, int pc_mod){
	assert(vm);
	assert(inst_ptr);
	assert(buf);

	value_t inst = inst_ptr->value;

	//0: HALT
	if(inst == 0){
		sprintf(buf,"HALT");
	}
	//1: SET REGISTER
	else if(inst == 1){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"SET ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_b->value));
	}
	//2: STACK PUSH
	else if(inst == 2){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"PUSH ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
	}
	//3: STACK POP
	else if(inst == 3){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"POP ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
	}
	//4: EQ OP
	else if(inst == 4){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);
		const cell * cell_c = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"EQ ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_c->value));
	}
	//5: GT OP
	else if(inst == 5){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);
		const cell * cell_c = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"GT ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_c->value));
	}
	//6: JUMP
	else if(inst == 6){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"JUMP ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
	}
	//7: JUMP IF TRUE
	else if(inst == 7){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"TJUMP ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_b->value));
	}
	//8: JUMP IF FALSE
	else if(inst == 8){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"FJUMP ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_b->value));
	}
	//9: ADD
	else if(inst == 9){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);
		const cell * cell_c = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"ADD ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_c->value));
	}
	//10: MULT
	else if(inst == 10){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);
		const cell * cell_c = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"MULT ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_c->value));
	}
	//11: MOD
	else if(inst == 11){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);
		const cell * cell_c = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"MOD ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_c->value));
	}
	//12: AND
	else if(inst == 12){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);
		const cell * cell_c = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"AND ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_c->value));
	}
	//13: OR
	else if(inst == 13){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);
		const cell * cell_c = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"OR ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_c->value));
	}
	//14: NOT
	else if(inst == 14){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"NOT ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_b->value));
	}
	//15: READ MEM
	else if(inst == 15){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"RMEM ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_b->value));
	}
	//16: WRITE MEM
	else if(inst == 16){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		const cell * cell_b = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"WMEM ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf+strlen(buf),"r%zu",Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 "",(cell_b->value));
	}
	//17: CALL
	else if(inst == 17){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"CALL ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
	}
	//18: RET
	else if(inst == 18){
		sprintf(buf,"RET");
	}
	//19: OUTPUT
	else if(inst == 19){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);
		value_t to_print = cell_a->value;

		sprintf(buf,"OUT ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else {
				if(((char*)&to_print)[0] == '\n') sprintf(buf+strlen(buf),"\\n"); 
				else sprintf(buf+strlen(buf),"%c",((char*)&to_print)[0]); 
			}
	}
	//20: INPUT
	else if(inst == 20){
		const cell * cell_a = Memory_get(vm->mem,++pc_mod);

		sprintf(buf,"IN ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf+strlen(buf),"r%zu ",Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf+strlen(buf),"%" PRIu16 " ",(cell_a->value));
	}
	//21: NO-OP
	else if(inst == 21){
		sprintf(buf,"NOOP");
	}
	else sprintf(buf,"%" PRIu16 " ",inst);

	return buf;
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


// write VM state to fp (stdout by default)
void state_dump(vm_t * vm){
	assert(vm);

	fprintf(vm->out_fp,"Current VM State: \n----------\n");

	fprintf(vm->out_fp,"Registers:\n");
	for(int i = 0; i < NUM_REGS; i++){
		fprintf(vm->out_fp,"\tr%d -> %" PRIu16 "\n",i,vm->regs[i]);
	}

	size_t stk_size = Stack_size(vm->stk);
	fprintf(vm->out_fp,"\nStack (size %zu):\n",stk_size);
	Stack * tmp = Stack_create();
	for(int i = 0; i < stk_size; i++){
		value_t v = Stack_pop(vm->stk);
		fprintf(vm->out_fp,"\ts%d -> %" PRIu16 "\n",i,v);
		Stack_push(tmp,v);
	}
	for(int i = 0; i < stk_size; i++){
		value_t v = Stack_pop(tmp);
		Stack_push(vm->stk,v);
	}
	assert(Stack_size(tmp) == 0);
	Stack_destroy(tmp);

	fprintf(vm->out_fp,"\nProgram Counter -> %zu\n",vm->pc);
}

// same as above, but also prints memory
void state_dump_full(vm_t * vm){
	state_dump(vm);
	size_t idx = 0;
	const cell * c = Memory_get(vm->mem,idx);

	fprintf(vm->out_fp,"\nMemory (size %zu):\n",Memory_size(vm->mem));
	while(c){
		fprintf(vm->out_fp,"\tm%" PRIu16 " -> %" PRIu16 "\n",c->addr,c->value);
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
