#ifndef __DBERGV_VM__
#define __DBERGV_VM__

#define NUM_REGS (8)
#define INPUT_FILENAME ("data/challenge.bin")
#define TRACE_FILENAME ("log/trace.log")
#define DECOMPILE_FILENAME ("log/decomp.txt")
#define DECOMPILE_FILENAME2 ("log/decomp_outs.txt") // record all "out" instruction data
#define DECOMPILE_FILENAME3 ("log/decomp_heap.txt") // record all instructionless data in memory
#define MAX_LOG_SIZE (50000000) // ~50 megabytes
#define FF_FILENAME ("script/fastforward")
#define MAX_VAL 32767

typedef struct vm_s {
	value_t * regs;
	Stack * stk;
	Memory * mem;
	size_t pc;

	char * stdin_buf;
	size_t stdin_idx;
	size_t stdin_buf_size;
	size_t stdin_str_len;
	char ff_flag;
	FILE * ff_script;
} vm_t;

void state_dump(vm_t * vm, FILE * fp);

void state_dump_full(vm_t * vm,FILE * fp);

value_t eval(vm_t * vm, value_t v);

#endif /* __DBERGV_VM__ */