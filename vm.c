#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "stack.h"
#include "value.h"
#include "mem.h"

#define NUM_REGS (8)
#define INPUT_FILENAME ("data/challenge.bin")
#define TRACE_FILENAME ("log/trace.log")
#define DECOMPILE_FILENAME ("log/decomp.txt")
#define DECOMPILE_FILENAME2 ("log/decomp_outs.txt") // record all "out" instruction data
#define DECOMPILE_FILENAME3 ("log/decomp_heap.txt") // record all instructionless data in memory
#define MAX_LOG_SIZE (50000000) // ~50 megabytes
#define FF_FILENAME ("script/fastforward")
#define MAX_VAL 32767

value_t * regs = NULL;
Stack * stk = NULL;
Memory * mem = NULL;
size_t pc = 0; //program counter

char * stdin_buf = NULL;
size_t stdin_idx = 1;
size_t stdin_buf_size = 0;
size_t stdin_str_len = 0;

char ff_flag = 0;
FILE * ff_script = NULL;

void endian_check(){
	volatile uint32_t i=0x01234567;
	if ((*((uint8_t*)(&i))) == 0x67)
		printf("Little E\n");
	else printf("Big E\n");
}

// do register lookups, if necessary
value_t eval(value_t v){
	if(v > MAX_VAL) return regs[Value_get_register_idx(&v)];
	else return v;
}

void load_binary(){
	assert(mem);

	FILE * fp;
	fp = fopen(INPUT_FILENAME,"rb");
	value_t buffer;

	while(fread(&buffer,sizeof(value_t),1,fp) > 0){
		//printf("%" PRIu16 " \n", buffer);
		cell c = {((uint16_t) Memory_size(mem)),buffer};

		Memory_append(mem,&c);
	}
    	
    fclose(fp);
}


// write VM state to fp (stdout by default)
void state_dump(FILE * fp){
	if(!fp)
		fp = stdout;

	printf("Current VM State: \n----------\n");

	printf("Registers:\n");
	for(int i = 0; i < NUM_REGS; i++){
		printf("\tr%d -> %" PRIu16 "\n",i,regs[i]);
	}

	size_t stk_size = Stack_size(stk);
	printf("\nStack (size %zu):\n",stk_size);
	Stack * tmp = Stack_create();
	for(int i = 0; i < stk_size; i++){
		value_t v = Stack_pop(stk);
		printf("\ts%d -> %" PRIu16 "\n",i,v);
		Stack_push(tmp,v);
	}
	for(int i = 0; i < stk_size; i++){
		value_t v = Stack_pop(tmp);
		Stack_push(stk,v);
	}
	assert(Stack_size(tmp) == 0);
	Stack_destroy(tmp);

	printf("\nProgram Counter -> %zu\n",pc);
}

// same as above, but also prints memory
void state_dump_full(FILE * fp){
	state_dump(fp);

	if(!fp)
		fp = stdout;

	size_t idx = 0;
	const cell * c = Memory_get(mem,idx);

	printf("\nMemory (size %zu):\n",Memory_size(mem));
	while(c){
		printf("\tm%" PRIu16 " -> %" PRIu16 "\n",c->addr,c->value);
		c = Memory_get(mem,++idx);
	}
}

void decompile(){
	assert(mem);

	size_t tmp = pc;
	pc = 0;

	//FILE * fp, fp2, fp3;
	FILE * fp = fopen(DECOMPILE_FILENAME,"w+");
	FILE * fp2 = fopen(DECOMPILE_FILENAME2,"w+");
	FILE * fp3 = fopen(DECOMPILE_FILENAME3,"w+");
	assert(fp);
	assert(fp2);
	assert(fp3);

	//size_t idx = 0;
	const cell * c = Memory_get(mem,pc);

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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			fprintf(fp,"SET ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//2: STACK PUSH
		else if(inst == 2){
			const cell * cell_a = Memory_get(mem,++pc);
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"PUSH r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"PUSH %" PRIu16 "",(cell_a->value));
		}
		//3: STACK POP
		else if(inst == 3){
			const cell * cell_a = Memory_get(mem,++pc);

			fprintf(fp,"POP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
		//4: EQ OP
		else if(inst == 4){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

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
			const cell * cell_a = Memory_get(mem,++pc);

			fprintf(fp,"JUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
		//7: JUMP IF TRUE
		else if(inst == 7){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			fprintf(fp,"TJUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//8: JUMP IF FALSE
		else if(inst == 8){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			fprintf(fp,"FJUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//9: ADD
		else if(inst == 9){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			fprintf(fp,"NOT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//15: READ MEM
		else if(inst == 15){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			const cell * read_cell = Memory_get(mem,eval(cell_b->value));

			fprintf(fp,"RMEM ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//16: WRITE MEM
		else if(inst == 16){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			fprintf(fp,"WMEM ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//17: CALL
		else if(inst == 17){
			const cell * cell_a = Memory_get(mem,++pc);

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
			const cell * cell_a = Memory_get(mem,++pc);
			value_t to_print = cell_a->value;

			fprintf(fp,"OUT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%c",((char*)&to_print)[0]); 
			if(Value_is_reg(&(cell_a->value))) fprintf(fp2,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp2,"%c%c",((char*)&to_print)[0],((char*)&to_print)[1]); 
		}
		//20: INPUT
		else if(inst == 20){
			const cell * cell_a = Memory_get(mem,++pc);

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
		c = Memory_get(mem,++pc);
	}

	fclose(fp);
	fclose(fp2);
	fclose(fp3);

	pc = tmp;
}

void execute_with_trace(){
	assert(mem);

	FILE * fp = fopen(TRACE_FILENAME,"w+");
	assert(fp);

	const cell * c_ptr = Memory_get(mem,pc);
	if(!c_ptr){
		exit(1);
	}


	while(c_ptr){
		if(ftell(fp) > MAX_LOG_SIZE){
			printf("---log size limit reached---\n");
			fprintf(fp,"---log size limit reached---\n");
			break;
		}
		value_t inst = c_ptr->value;
		//printf("%" PRIu16 " %" PRIu16 "\n",c_ptr->addr,c_ptr->value);
		fprintf(fp,"[%" PRIu16 "] ",c_ptr->addr);

		//0: HALT
		if(inst == 0){
			fprintf(fp,"HALT");
			return;
		}
		//1: SET REGISTER
		else if(inst == 1){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = eval(cell_b->value);
			fprintf(fp,"SET r%zu %hu",Value_get_register_idx(&(cell_a->value)),eval(cell_b->value));
		}
		//2: STACK PUSH
		else if(inst == 2){
			const cell * cell_a = Memory_get(mem,++pc);

			Stack_push(stk,eval(cell_a->value));
			fprintf(fp,"PUSH %" PRIu16 "",eval(cell_a->value));
		}
		//3: STACK POP
		else if(inst == 3){
			const cell * cell_a = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = Stack_pop(stk);
			fprintf(fp,"POP r%zu (%hu)",Value_get_register_idx(&(cell_a->value)),regs[Value_get_register_idx(&(cell_a->value))]);
		}
		//4: EQ OP
		else if(inst == 4){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			size_t a_idx = Value_get_register_idx(&(cell_a->value));

			if(eval(cell_b->value) == eval(cell_c->value)) regs[a_idx] = 1;
			else regs[a_idx] = 0;
			fprintf(fp,"EQ ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",eval(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 " ",eval(cell_b->value));
			if(Value_is_reg(&(cell_c->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_c->value)));
			else fprintf(fp,"%" PRIu16 "",eval(cell_c->value));

		}
		//5: GT OP
		else if(inst == 5){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			size_t a_idx = Value_get_register_idx(&(cell_a->value));

			if(eval(cell_b->value) > eval(cell_c->value)) regs[a_idx] = 1;
			else regs[a_idx] = 0;
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
			const cell * cell_a = Memory_get(mem,++pc);

			pc = eval(cell_a->value)-1;
			fprintf(fp,"JUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
		//7: JUMP IF TRUE
		else if(inst == 7){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			pc = (eval(cell_a->value)) ? (eval(cell_b->value)-1) : pc;
			fprintf(fp,"TJUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//8: JUMP IF FALSE
		else if(inst == 8){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);


			pc = (!eval(cell_a->value)) ? (eval(cell_b->value)-1) : pc;
			fprintf(fp,"FJUMP ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//9: ADD
		else if(inst == 9){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) + eval(cell_c->value)) % 32768;
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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) * eval(cell_c->value)) % 32768;
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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) % eval(cell_c->value));
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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) & eval(cell_c->value));
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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) | eval(cell_c->value));
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
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = Value_bitwise_not(eval(cell_b->value));
			fprintf(fp,"NOT ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//15: READ MEM
		else if(inst == 15){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			const cell * read_cell = Memory_get(mem,eval(cell_b->value));

			regs[Value_get_register_idx(&(cell_a->value))] = read_cell->value;
			fprintf(fp,"RMEM ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//16: WRITE MEM
		else if(inst == 16){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			cell write_cell = {eval(cell_a->value),eval(cell_b->value)};

			Memory_set(mem,eval(cell_a->value),&write_cell);
			fprintf(fp,"WMEM ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu ",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 " ",(cell_a->value));
			if(Value_is_reg(&(cell_b->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_b->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_b->value));
		}
		//17: CALL
		else if(inst == 17){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			//printf("call %" PRIu16 " %" PRIu16 " %" PRIu16 "\n",cell_a->addr,cell_a->value,eval(cell_a->value));
			Stack_push(stk,eval(cell_b->addr));
			pc = eval(cell_a->value) - 1;
			fprintf(fp,"CALL ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
		//18: RET
		else if(inst == 18){
			value_t ret_addr = Stack_pop(stk);

			pc = ret_addr - 1;
			fprintf(fp,"RET");
		}
		//19: OUTPUT
		else if(inst == 19){
			const cell * cell_a = Memory_get(mem,++pc);

			value_t to_print = eval(cell_a->value);
			printf("%c%c",((char*)&to_print)[0],((char*)&to_print)[1]);
			fprintf(fp, "OUT %c%c",((char*)&to_print)[0],((char*)&to_print)[1]);
			//fprintf(fp,"OUT ");
			//if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			//else fprintf(fp,"%c",((char*)&to_print)[0]); 
		}
		//20: INPUT
		else if(inst == 20){
			const cell * cell_a = Memory_get(mem,++pc);

			if(!(stdin_idx < stdin_str_len)){
				if(stdin_buf){
					free(stdin_buf);
					stdin_buf = NULL;
				}
				if(ff_flag){
					assert(ff_script);
					stdin_str_len = getline(&stdin_buf,&stdin_buf_size,ff_script);
					if(stdin_str_len == -1){ // end of file
						ff_flag = 0;
						if(stdin_buf){
							free(stdin_buf);
							stdin_buf = NULL;
						}
						fclose(ff_script);
						ff_script = NULL;
					}
				}
				if(!ff_flag)
					stdin_str_len = getline(&stdin_buf,&stdin_buf_size,stdin);
				stdin_idx = 0;
				//stdin_buf[stdin_str_len] = '\0';

				//printf("Read %zu bytes\n",stdin_str_len);
			}
			if(strcmp(stdin_buf,"dump\n") == 0){
				state_dump(NULL);
			}
			if(strcmp(stdin_buf,"fdump\n") == 0){
				state_dump_full(NULL);
			}
			if((strcmp(stdin_buf,"ff\n") == 0) || (strcmp(stdin_buf,"fastforward\n") == 0)){
				ff_script = fopen(FF_FILENAME,"r");
				assert(ff_script);
				ff_flag = 1;
			}
			if((strcmp(stdin_buf,"q\n") == 0) || (strcmp(stdin_buf,"quit\n") == 0) || (strcmp(stdin_buf,"exit\n") == 0)){
				fprintf(fp,"USER_EXIT\n");
				fclose(fp);
				return;
			}
			if(stdin_buf[0] == '_'){ 
				if(strcmp(strtok(stdin_buf," "),"_setreg") == 0){
					size_t reg_id = atoi(strtok(NULL," "));
					value_t val = (value_t) atoi(strtok(NULL,"\n"));
					regs[reg_id] = val;
					stdin_idx = stdin_str_len; // trigger new read next time around

					//decompile();
				}
			}
			else regs[Value_get_register_idx(&(cell_a->value))] = (value_t) stdin_buf[stdin_idx++];

			fprintf(fp,"IN ");
			if(Value_is_reg(&(cell_a->value))) fprintf(fp,"r%zu",Value_get_register_idx(&(cell_a->value)));
			else fprintf(fp,"%" PRIu16 "",(cell_a->value));
		}
		//21: NO-OP
		else if(inst == 21){
			fprintf(fp,"NOOP");
		}
		else{
			fprintf(fp,"%" PRIu16 " ",inst);
			fprintf(fp,"(%c%c)",((char*)&inst)[0],((char*)&inst)[1]); 
		}

		fprintf(fp,"\n");
		c_ptr = Memory_get(mem,++pc);
	}

	fclose(fp);
	
	printf("\n");
}

void init_vm(){
	assert(!regs);
	assert(!stk);

	regs = calloc(NUM_REGS,sizeof(value_t));
	stk = Stack_create();
	mem = Memory_create();
	//stdin_buf = malloc(INPUT_BUFFER_SIZE);
	load_binary();
}

void shutdown_vm(){
	assert(regs);
	assert(stk);

	free(regs);
	Stack_destroy(stk);
	Memory_destroy(mem);
	if(stdin_buf) free(stdin_buf);
}


void execute(){
	assert(mem);

	const cell * c_ptr = Memory_get(mem,pc);
	if(!c_ptr){
		exit(1);
	}

	while(c_ptr){
		value_t inst = c_ptr->value;
		//printf("%" PRIu16 " %" PRIu16 "\n",c_ptr->addr,c_ptr->value);

		//0: HALT
		if(inst == 0){
			return;
		}
		//1: SET REGISTER
		else if(inst == 1){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = eval(cell_b->value);
		}
		//2: STACK PUSH
		else if(inst == 2){
			const cell * cell_a = Memory_get(mem,++pc);

			Stack_push(stk,eval(cell_a->value));
		}
		//3: STACK POP
		else if(inst == 3){
			const cell * cell_a = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = Stack_pop(stk);
		}
		//4: EQ OP
		else if(inst == 4){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			size_t a_idx = Value_get_register_idx(&(cell_a->value));

			if(eval(cell_b->value) == eval(cell_c->value)) regs[a_idx] = 1;
			else regs[a_idx] = 0;
		}
		//5: GT OP
		else if(inst == 5){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			size_t a_idx = Value_get_register_idx(&(cell_a->value));

			if(eval(cell_b->value) > eval(cell_c->value)) regs[a_idx] = 1;
			else regs[a_idx] = 0;
		}
		//6: JUMP
		else if(inst == 6){
			const cell * cell_a = Memory_get(mem,++pc);

			pc = eval(cell_a->value)-1;
		}
		//7: JUMP IF TRUE
		else if(inst == 7){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);


			pc = (eval(cell_a->value)) ? (eval(cell_b->value)-1) : pc;
		}
		//8: JUMP IF FALSE
		else if(inst == 8){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);


			pc = (!eval(cell_a->value)) ? (eval(cell_b->value)-1) : pc;
		}
		//9: ADD
		else if(inst == 9){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) + eval(cell_c->value)) % 32768;
		}
		//10: MULT
		else if(inst == 10){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) * eval(cell_c->value)) % 32768;
		}
		//11: MOD
		else if(inst == 11){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) % eval(cell_c->value));
		}
		//12: AND
		else if(inst == 12){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) & eval(cell_c->value));
		}
		//13: OR
		else if(inst == 13){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);
			const cell * cell_c = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = (eval(cell_b->value) | eval(cell_c->value));
		}
		//14: NOT
		else if(inst == 14){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			regs[Value_get_register_idx(&(cell_a->value))] = Value_bitwise_not(eval(cell_b->value));
		}
		//15: READ MEM
		else if(inst == 15){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			const cell * read_cell = Memory_get(mem,eval(cell_b->value));

			regs[Value_get_register_idx(&(cell_a->value))] = read_cell->value;
		}
		//16: WRITE MEM
		else if(inst == 16){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			cell write_cell = {eval(cell_a->value),eval(cell_b->value)};

			Memory_set(mem,eval(cell_a->value),&write_cell);
		}
		//17: CALL
		else if(inst == 17){
			const cell * cell_a = Memory_get(mem,++pc);
			const cell * cell_b = Memory_get(mem,++pc);

			//printf("call %" PRIu16 " %" PRIu16 " %" PRIu16 "\n",cell_a->addr,cell_a->value,eval(cell_a->value));
			Stack_push(stk,eval(cell_b->addr));
			pc = eval(cell_a->value) - 1;
		}
		//18: RET
		else if(inst == 18){
			value_t ret_addr = Stack_pop(stk);

			pc = ret_addr - 1;
		}
		//19: OUTPUT
		else if(inst == 19){
			const cell * cell_a = Memory_get(mem,++pc);

			value_t to_print = eval(cell_a->value);
			printf("%c%c",((char*)&to_print)[0],((char*)&to_print)[1]);
		}
		//20: INPUT
		else if(inst == 20){
			const cell * cell_a = Memory_get(mem,++pc);

			if(!(stdin_idx < stdin_str_len)){
				if(stdin_buf){
					free(stdin_buf);
					stdin_buf = NULL;
				}
				if(ff_flag){
					assert(ff_script);
					stdin_str_len = getline(&stdin_buf,&stdin_buf_size,ff_script);
					if(stdin_str_len == -1){ // end of file
						ff_flag = 0;
						if(stdin_buf){
							free(stdin_buf);
							stdin_buf = NULL;
						}
						fclose(ff_script);
						ff_script = NULL;
					}
				}
				if(!ff_flag)
					stdin_str_len = getline(&stdin_buf,&stdin_buf_size,stdin);
				stdin_idx = 0;
				//stdin_buf[stdin_str_len] = '\0';

				//printf("Read %zu bytes\n",stdin_str_len);
			}
			if(strcmp(stdin_buf,"dump\n") == 0){
				state_dump(NULL);
			}
			if(strcmp(stdin_buf,"fdump\n") == 0){
				state_dump_full(NULL);
			}
			if((strcmp(stdin_buf,"ff\n") == 0) || (strcmp(stdin_buf,"fastforward\n") == 0)){
				ff_script = fopen("fastforward","r");
				assert(ff_script);
				ff_flag = 1;
			}
			if((strcmp(stdin_buf,"q\n") == 0) || (strcmp(stdin_buf,"quit\n") == 0) || (strcmp(stdin_buf,"exit\n") == 0)){
				return;
			}
			if(stdin_buf[0] == '_'){ 
				if(strcmp(strtok(stdin_buf," "),"_setreg") == 0){
					size_t reg_id = atoi(strtok(NULL," "));
					value_t val = (value_t) atoi(strtok(NULL,"\n"));
					regs[reg_id] = val;
					stdin_idx = stdin_str_len; // trigger new read next time around

					//decompile();
				}
			}
			else regs[Value_get_register_idx(&(cell_a->value))] = (value_t) stdin_buf[stdin_idx++];
		}
		//21: NO-OP
		else if(inst == 21){

		}


		c_ptr = Memory_get(mem,++pc);
	}
	
	printf("\n");

}


int main(){
	init_vm();

	//execute();
	//decompile();
	execute_with_trace();

	shutdown_vm();
	return 0;
}