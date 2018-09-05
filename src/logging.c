//
// Created by David Bergvelt on 9/5/18.
//

#include "logging.h"
#include <assert.h>
#include <string.h>
#include <inttypes.h>

// gives string representation of memory cells (e.g. turns opcodes into legible instructions)
// assumes buf contains adequate space to store characters
// updates inst_ptr to the cell immediately following the last argument for the given instruction
char * string_of_cell(struct vm_s * vm, const cell * inst_ptr, char * buf, int * pc_ptr){
	assert(vm);
	assert(inst_ptr);
	assert(buf);

	uint16_t inst = inst_ptr->value;

	//0: HALT
	if(inst == 0){
		sprintf(buf,"HALT");
	}
	//1: SET REGISTER
	else if(inst == 1){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"SET ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_b->value));
	}
	//2: STACK PUSH
	else if(inst == 2){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"PUSH ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
	}
	//3: STACK POP
	else if(inst == 3){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"POP ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
	}
	//4: EQ OP
	else if(inst == 4){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_c = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"EQ ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_c->value));
	}
	//5: GT OP
	else if(inst == 5){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_c = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"GT ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_c->value));
	}
	//6: JUMP
	else if(inst == 6){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"JUMP ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
	}
	//7: JUMP IF TRUE
	else if(inst == 7){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"TJUMP ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_b->value));
	}
	//8: JUMP IF FALSE
	else if(inst == 8){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"FJUMP ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_b->value));
	}
	//9: ADD
	else if(inst == 9){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_c = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"ADD ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_c->value));
	}
	//10: MULT
	else if(inst == 10){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_c = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"MULT ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_c->value));
	}
	//11: MOD
	else if(inst == 11){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_c = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"MOD ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_c->value));
	}
	//12: AND
	else if(inst == 12){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_c = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"AND ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_c->value));
	}
	//13: OR
	else if(inst == 13){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_c = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"OR ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_b->value));
		if(Value_is_reg(&(cell_c->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_c->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_c->value));
	}
	//14: NOT
	else if(inst == 14){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"NOT ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_b->value));
	}
	//15: READ MEM
	else if(inst == 15){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"RMEM ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_b->value));
	}
	//16: WRITE MEM
	else if(inst == 16){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		const cell * cell_b = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"WMEM ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
		if(Value_is_reg(&(cell_b->value))) sprintf(buf + strlen(buf), "r%zu", Value_get_register_idx(&(cell_b->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 "",(cell_b->value));
	}
	//17: CALL
	else if(inst == 17){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"CALL ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
	}
	//18: RET
	else if(inst == 18){
		sprintf(buf,"RET");
	}
	//19: OUTPUT
	else if(inst == 19){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));
		uint16_t to_print = cell_a->value;

		sprintf(buf,"OUT ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else {
				if(((char*)&to_print)[0] == '\n') sprintf(buf + strlen(buf), "\\n");
				else sprintf(buf + strlen(buf), "%c", ((char*)&to_print)[0]);
			}
	}
	//20: INPUT
	else if(inst == 20){
		const cell * cell_a = Memory_get(vm->mem, ++(*pc_ptr));

		sprintf(buf,"IN ");
		if(Value_is_reg(&(cell_a->value))) sprintf(buf + strlen(buf), "r%zu ", Value_get_register_idx(&(cell_a->value)));
		else sprintf(buf + strlen(buf), "%" PRIu16 " ",(cell_a->value));
	}
	//21: NO-OP
	else if(inst == 21){
		sprintf(buf,"NOOP");
	}
	else sprintf(buf,"%" PRIu16 " ",inst);

	return buf;
}

// write VM state to fp (stdout by default)
void state_dump(struct vm_s * vm){
	assert(vm);

	fprintf(vm->out_fp,"Current VM State: \n----------\n");

	fprintf(vm->out_fp,"Registers:\n");
	for(int i = 0; i < NUM_REGS; i++){
		fprintf(vm->out_fp,"\tr%d -> %" PRIu16 "\n",i,vm->regs[i]);
	}

	__darwin_size_t stk_size = Stack_size(vm->stk);
	fprintf(vm->out_fp,"\nStack (size %zu):\n",stk_size);
	Stack * tmp = Stack_create();
	for(int i = 0; i < stk_size; i++){
		uint16_t v = Stack_pop(vm->stk);
		fprintf(vm->out_fp,"\ts%d -> %" PRIu16 "\n",i,v);
		Stack_push(tmp,v);
	}
	for(int i = 0; i < stk_size; i++){
		uint16_t v = Stack_pop(tmp);
		Stack_push(vm->stk,v);
	}
	assert(Stack_size(tmp) == 0);
	Stack_destroy(tmp);

	fprintf(vm->out_fp,"\nProgram Counter -> %zu\n",vm->pc);
}

// same as above, but also prints memory
void state_dump_full(struct vm_s * vm){
	state_dump(vm);
	__darwin_size_t idx = 0;
	const cell * c = Memory_get(vm->mem, idx);

	fprintf(vm->out_fp, "\nMemory (size %zu):\n", Memory_size(vm->mem));
	while(c){
		fprintf(vm->out_fp,"\tm%" PRIu16 " -> %" PRIu16 "\n",c->addr,c->value);
		c = Memory_get(vm->mem,++idx);
	}
}

void decompile(vm_t * vm){
	assert(vm);

	value_t * regs = vm->regs;
	Stack * stk = vm->stk;
	Memory * mem = vm->mem;

	size_t tmp = vm->pc;
	vm->pc = 0;

	//FILE * fp, fp2, fp3;
	FILE * fp = fopen(DECOMPILE_FILENAME,"w+");
	FILE * fp2 = fopen(DECOMPILE_FILENAME2,"w+");
	FILE * fp3 = fopen(DECOMPILE_FILENAME3,"w+");
	assert(fp);
	assert(fp2);
	assert(fp3);

	//size_t idx = 0;
	const cell * c = Memory_get(mem,vm->pc);

	while(c){
		value_t inst = c->value;
		//fprintf(stdout,"%" PRIu16 "\n",inst);
		fprintf(fp,"[%" PRIu16 "] ",c->addr);

		//0: HALT
		if(inst == 0){
			fprintf(fp,"HALT");
		}
		//1: SET REGISTER
		else if(inst == 1){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));

			fprintf(fp,"SET ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//2: STACK PUSH
		else if(inst == 2){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"PUSH r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"PUSH %" PRIu16 "",(cell_a->value));
		}
		//3: STACK POP
		else if(inst == 3){
			const cell * cell_a = Memory_get(mem,++(vm->pc));

			fprintf(fp,"POP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
		//4: EQ OP
		else if(inst == 4){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));
			const cell * cell_c = Memory_get(mem,++(vm->pc));

			fprintf(fp,"EQ ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
		//5: GT OP
		else if(inst == 5){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));
			const cell * cell_c = Memory_get(mem,++(vm->pc));

			fprintf(fp,"GT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
		//6: JUMP
		else if(inst == 6){
			const cell * cell_a = Memory_get(mem,++(vm->pc));

			fprintf(fp,"JUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
		//7: JUMP IF TRUE
		else if(inst == 7){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));

			fprintf(fp,"TJUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//8: JUMP IF FALSE
		else if(inst == 8){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));

			fprintf(fp,"FJUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//9: ADD
		else if(inst == 9){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));
			const cell * cell_c = Memory_get(mem,++(vm->pc));

			fprintf(fp,"ADD ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
		//10: MULT
		else if(inst == 10){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));
			const cell * cell_c = Memory_get(mem,++(vm->pc));

			fprintf(fp,"MULT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
		//11: MOD
		else if(inst == 11){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));
			const cell * cell_c = Memory_get(mem,++(vm->pc));

			fprintf(fp,"MOD ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
		//12: AND
		else if(inst == 12){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));
			const cell * cell_c = Memory_get(mem,++(vm->pc));

			fprintf(fp,"AND ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
		//13: OR
		else if(inst == 13){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));
			const cell * cell_c = Memory_get(mem,++(vm->pc));

			fprintf(fp,"OR ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
		//14: NOT
		else if(inst == 14){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));

			fprintf(fp,"NOT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//15: READ MEM
		else if(inst == 15){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));

			const cell * read_cell = Memory_get(mem,eval(vm,cell_b->value));

			fprintf(fp,"RMEM ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//16: WRITE MEM
		else if(inst == 16){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			const cell * cell_b = Memory_get(mem,++(vm->pc));

			fprintf(fp,"WMEM ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//17: CALL
		else if(inst == 17){
			const cell * cell_a = Memory_get(mem,++(vm->pc));

			fprintf(fp,"CALL ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
		//18: RET
		else if(inst == 18){
			fprintf(fp,"RET");
		}
		//19: OUTPUT
		else if(inst == 19){
			const cell * cell_a = Memory_get(mem,++(vm->pc));
			value_t to_print = cell_a->value;

			fprintf(fp,"OUT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else {
				if(((char*)&to_print)[0] == '\n') fprintf(fp,"\\n");
				else fprintf(fp,"%c",((char*)&to_print)[0]);
			}
			if(Value_is_reg(&(cell_a->value))) fprintf(fp2,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp2,"%c%c",((char*)&to_print)[0],((char*)&to_print)[1]);
		}
		//20: INPUT
		else if(inst == 20){
			const cell * cell_a = Memory_get(mem,++(vm->pc));

			fprintf(fp,"IN ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
		//21: NO-OP
		else if(inst == 21){
			fprintf(fp,"NOOP");
		}
		//No valid inst... just print data
		else{
			fprintf(fp,"%" PRIu16 " ",inst);
			fprintf(fp,"(%c%c)",((char*)&inst)[0],((char*)&inst)[1]);

			//fprintf(fp3,"%" PRIu16 " ",inst);
			//fprintf(fp3,"%c%c",((char*)&inst)[0],((char*)&inst)[1]);
			fprintf(fp3,"%c",((char*)&inst)[0]);
		}

		fprintf(fp,"\n");
		c = Memory_get(mem,++(vm->pc));
	}

	fclose(fp);
	fclose(fp2);
	fclose(fp3);

	vm->pc = tmp;
}