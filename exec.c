#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "stack.h"
#include "value.h"
#include "mem.h"
#include "vm.h"
#include "exec.h"

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

const cell * exec_step(vm_t * vm, FILE * fp, const cell * c_ptr, int * breakflag){
	// these can be abbreviated, since we do not expect the pointer locations to change during execution
	// this cannot be so easily done for VM struct members such as the program counter, since we will have
	// separate values for the PC in this function and in the VM struct
	value_t * regs = vm->regs;
	Stack * stk = vm->stk;
	Memory * mem = vm->mem;

	if(fp && (ftell(fp) > MAX_LOG_SIZE)){
		fprintf(vm->out_fp,"---log size limit reached---\n");
		fprintf(fp,"---log size limit reached---\n");
		return NULL;
	}
	value_t inst = c_ptr->value;
	if(fp) fprintf(fp,"[%" PRIu16 "] ",c_ptr->addr);

	//0: HALT
	if(inst == 0){
		if(fp) fprintf(fp,"HALT");
		return NULL;
	}
	//1: SET REGISTER
	else if(inst == 1){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));

		regs[Value_get_register_idx(&(cell_a->value))] = eval(vm,cell_b->value);
		if(fp) fprintf(fp,"SET r%zu %hu",Value_get_register_idx(&(cell_a->value)),eval(vm,cell_b->value));
	}
	//2: STACK PUSH
	else if(inst == 2){
		const cell * cell_a = Memory_get(mem,++(vm->pc));

		Stack_push(stk,eval(vm,cell_a->value));
		if(fp) fprintf(fp,"PUSH %" PRIu16 "",eval(vm,cell_a->value));
	}
	//3: STACK POP
	else if(inst == 3){
		const cell * cell_a = Memory_get(mem,++(vm->pc));

		regs[Value_get_register_idx(&(cell_a->value))] = Stack_pop(stk);
		if(fp) fprintf(fp,"POP %" PRIu16 "",eval(vm,cell_a->value));
	}
	//4: EQ OP
	else if(inst == 4){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));
		const cell * cell_c = Memory_get(mem,++(vm->pc));

		size_t a_idx = Value_get_register_idx(&(cell_a->value));

		if(eval(vm,cell_b->value) == eval(vm,cell_c->value)) regs[a_idx] = 1;
		else regs[a_idx] = 0;
		if(fp){
			fprintf(fp,"EQ ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",eval(vm,cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",eval(vm,cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",eval(vm,cell_c->value));
		}
	}
	//5: GT OP
	else if(inst == 5){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));
		const cell * cell_c = Memory_get(mem,++(vm->pc));

		size_t a_idx = Value_get_register_idx(&(cell_a->value));

		if(eval(vm,cell_b->value) > eval(vm,cell_c->value)) regs[a_idx] = 1;
		else regs[a_idx] = 0;
		if(fp){
			fprintf(fp,"GT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
	}
	//6: JUMP
	else if(inst == 6){
		const cell * cell_a = Memory_get(mem,++(vm->pc));

		vm->pc = eval(vm,cell_a->value)-1;
		if(fp){
			fprintf(fp,"JUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
	}
	//7: JUMP IF TRUE
	else if(inst == 7){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));


		vm->pc = (eval(vm,cell_a->value)) ? (eval(vm,cell_b->value)-1) : vm->pc;
		if(fp){
			fprintf(fp,"TJUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
	}
	//8: JUMP IF FALSE
	else if(inst == 8){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));


		vm->pc = (!eval(vm,cell_a->value)) ? (eval(vm,cell_b->value)-1) : vm->pc;
		if(fp){
			fprintf(fp,"FJUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
	}
	//9: ADD
	else if(inst == 9){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));
		const cell * cell_c = Memory_get(mem,++(vm->pc));

		regs[Value_get_register_idx(&(cell_a->value))] = (eval(vm,cell_b->value) + eval(vm,cell_c->value)) % 32768;
		if(fp){
			fprintf(fp,"ADD ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
	}
	//10: MULT
	else if(inst == 10){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));
		const cell * cell_c = Memory_get(mem,++(vm->pc));

		regs[Value_get_register_idx(&(cell_a->value))] = (eval(vm,cell_b->value) * eval(vm,cell_c->value)) % 32768;
		if(fp){
			fprintf(fp,"MULT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
	}
	//11: MOD
	else if(inst == 11){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));
		const cell * cell_c = Memory_get(mem,++(vm->pc));

		regs[Value_get_register_idx(&(cell_a->value))] = (eval(vm,cell_b->value) % eval(vm,cell_c->value));
		if(fp){
			fprintf(fp,"MOD ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
	}
	//12: AND
	else if(inst == 12){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));
		const cell * cell_c = Memory_get(mem,++(vm->pc));

		regs[Value_get_register_idx(&(cell_a->value))] = (eval(vm,cell_b->value) & eval(vm,cell_c->value));
		if(fp){
			fprintf(fp,"AND ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
	}
	//13: OR
	else if(inst == 13){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));
		const cell * cell_c = Memory_get(mem,++(vm->pc));

		regs[Value_get_register_idx(&(cell_a->value))] = (eval(vm,cell_b->value) | eval(vm,cell_c->value));
		if(fp){
			fprintf(fp,"OR ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_c->value));
		}
	}
	//14: NOT
	else if(inst == 14){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));

		regs[Value_get_register_idx(&(cell_a->value))] = Value_bitwise_not(eval(vm,cell_b->value));
		if(fp){
			fprintf(fp,"NOT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
	}
	//15: READ MEM
	else if(inst == 15){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));

		const cell * read_cell = Memory_get(mem,eval(vm,cell_b->value));

		regs[Value_get_register_idx(&(cell_a->value))] = read_cell->value;
		if(fp){
			fprintf(fp,"RMEM ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
	}
	//16: WRITE MEM
	else if(inst == 16){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));

		cell write_cell = {eval(vm,cell_a->value),eval(vm,cell_b->value)};

		Memory_set(mem,eval(vm,cell_a->value),&write_cell);
		if(fp){
			fprintf(fp,"WMEM ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
	}
	//17: CALL
	else if(inst == 17){
		const cell * cell_a = Memory_get(mem,++(vm->pc));
		const cell * cell_b = Memory_get(mem,++(vm->pc));

		//printf("call %" PRIu16 " %" PRIu16 " %" PRIu16 "\n",cell_a->addr,cell_a->value,eval(vm,cell_a->value));
		Stack_push(stk,eval(vm,cell_b->addr));
		vm->pc = eval(vm,cell_a->value) - 1;
		if(fp){
			fprintf(fp,"CALL ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
	}
	//18: RET
	else if(inst == 18){
		value_t ret_addr = Stack_pop(stk);

		vm->pc = ret_addr - 1;
		if(fp) fprintf(fp,"RET");
	}
	//19: OUTPUT
	else if(inst == 19){
		const cell * cell_a = Memory_get(mem,++(vm->pc));

		value_t to_print = eval(vm,cell_a->value);
		//fprintf(vm->out_fp,"%c%c",((char*)&to_print)[0],((char*)&to_print)[1]);
		if(vm->out_fd < 0)
			fprintf(vm->out_fp,"%c%c",((char*)&to_print)[0],((char*)&to_print)[1]);
		else{
			dprintf(vm->out_fd,"%c%c",((char*)&to_print)[0],((char*)&to_print)[1]);
		}
		if(fp) fprintf(fp, "OUT %c%c",((char*)&to_print)[0],((char*)&to_print)[1]);
	}
	//20: INPUT
	else if(inst == 20){
		const cell * cell_a = Memory_get(mem,++(vm->pc));

		if(!(vm->stdin_idx < vm->stdin_str_len)){
			if(vm->stdin_buf){
				free(vm->stdin_buf);
				vm->stdin_buf = NULL;
			}
			if(vm->ff_flag){
				assert(vm->ff_script);
				vm->stdin_str_len = getline(&vm->stdin_buf,&vm->stdin_buf_size,vm->ff_script);
				if(vm->stdin_str_len == -1){ // end of file
					vm->ff_flag = 0;
					if(vm->stdin_buf){
						free(vm->stdin_buf);
						vm->stdin_buf = NULL;
					}
					fclose(vm->ff_script);
					vm->ff_script = NULL;
				}
			}
			if(!vm->ff_flag){
				if(vm->in_fd < 0)
					vm->stdin_str_len = getline(&vm->stdin_buf,&vm->stdin_buf_size,vm->in_fp);
				else{
					// read from pipe
					vm->stdin_buf = calloc(256,1);
					vm->stdin_buf_size = 255;
					vm->stdin_idx = 0;
					vm->stdin_str_len = 0;
					while (read(vm->in_fd,vm->stdin_buf+vm->stdin_str_len, 1) > 0){
						vm->stdin_str_len++;
						if (vm->stdin_str_len > vm->stdin_buf_size)
							break; // buffer overflow

						if (vm->stdin_buf[vm->stdin_str_len-1] == '\n')
							break; // end of line
					}
				}
				if(breakflag) *breakflag = 1;
			}
			vm->stdin_idx = 0;

			//printf("Read %zu bytes\n",vm->stdin_str_len);
		}
		if(strcmp(vm->stdin_buf,"dump\n") == 0){
			state_dump(vm);
		}
		if(strcmp(vm->stdin_buf,"fdump\n") == 0){
			state_dump_full(vm);
		}
		if((strcmp(vm->stdin_buf,"ff\n") == 0) || (strcmp(vm->stdin_buf,"fastforward\n") == 0)){
			vm->ff_script = fopen(FF_FILENAME,"r");
			assert(vm->ff_script);
			vm->ff_flag = 1;
		}
		if((strcmp(vm->stdin_buf,"q\n") == 0) || (strcmp(vm->stdin_buf,"quit\n") == 0) || (strcmp(vm->stdin_buf,"exit\n") == 0)){
			if(fp){
				fprintf(fp,"USER_EXIT\n");
				fclose(fp);
				fp = NULL;
			}
			return NULL;
		}
		if(strcmp(vm->stdin_buf,"tele_hack\n") == 0){
			if(fp){
				fprintf(fp,"--- modifying teleporter check code in memory ---\n");
			}
			//[5483] SET r0 4 --> [5483] SET r0 6
			cell overwrite_cell1 = {(value_t) 5485,(value_t) 6};
			Memory_set(mem,5485,&overwrite_cell1);

			//[6027] TJUMP r0 6035 --> [6027] RET
			cell overwrite_cell2 = {(value_t) 6027,(value_t) 18};
			Memory_set(mem,6027,&overwrite_cell2);

		}
		if(vm->stdin_buf[0] == '_'){ 
			if(strcmp(strtok(vm->stdin_buf," "),"_setreg") == 0){
				size_t reg_id = atoi(strtok(NULL," "));
				value_t val = (value_t) atoi(strtok(NULL,"\n"));
				regs[reg_id] = val;
				vm->stdin_idx = vm->stdin_str_len; // trigger new read next time around
			}
		}
		else regs[Value_get_register_idx(&(cell_a->value))] = (value_t) vm->stdin_buf[vm->stdin_idx++];

		if(fp){
			fprintf(fp,"IN ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
	}
	//21: NO-OP
	else if(inst == 21){
		if(fp) fprintf(fp,"NOOP");
	}
	else if(fp){
		fprintf(fp,"%" PRIu16 " ",inst);
		fprintf(fp,"(%c%c)",((char*)&inst)[0],((char*)&inst)[1]); 
	}

	if(fp) fprintf(fp,"\n");
	const cell * c_ptr_ret = Memory_get(mem,++(vm->pc));
	return c_ptr_ret;
}

void execute_debug(vm_t * vm, FILE * breakpoint_fp, int * breakflag){
	assert(vm);
	assert(breakpoint_fp);
	assert(breakflag);

	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int breakpoint_count = 0;

    // count number of breakpoints
    while(!feof(breakpoint_fp)){
  		char ch = fgetc(breakpoint_fp);
  		if(ch == '\n'){
			breakpoint_count++;
		}
	}
	int * breakpoints = malloc(breakpoint_count*sizeof(int)); // FIX THIS! (currently being malloc'd lots of times)
	int curr_idx = 0;
	fseek(breakpoint_fp, 0, SEEK_SET);

    while ((read = getline(&line, &len, breakpoint_fp)) != -1) {
    	breakpoints[curr_idx] = atoi(line);
    	if(line){
    		free(line);
    		line = NULL;
    	} 
    	curr_idx++;
    }
    if (line) free(line);

	const cell * inst_ptr = Memory_get(vm->mem,vm->pc);
	if(!inst_ptr){
		exit(1);
	}

	while(inst_ptr){
		for(int i = 0; i < breakpoint_count; i++){
			if(inst_ptr->addr == ((value_t) breakpoints[i])){
				*breakflag = (*breakflag) ? 0 : 1;
			}
		}
		if(*breakflag) break;

		inst_ptr = exec_step(vm,NULL,inst_ptr,breakflag);
		if(!inst_ptr) *breakflag = -1;
	}

	free(breakpoints);

	return;
}

void execute(vm_t * vm, int log_flag){
	assert(vm);

	FILE * fp = NULL;
	if(log_flag == LOG_TRACE){
		fp = fopen(TRACE_FILENAME,"w+");
		assert(fp);
	}

	const cell * inst_ptr = Memory_get(vm->mem,vm->pc);
	if(!inst_ptr){
		exit(1);
	}

	while(inst_ptr){
		inst_ptr = exec_step(vm,fp,inst_ptr,NULL);
	}
	if(fp) fclose(fp);

	printf("\n");
}