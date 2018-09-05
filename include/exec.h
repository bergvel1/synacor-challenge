#ifndef __DBERGV_EXEC__
#define __DBERGV_EXEC__

#define NO_LOG 0
#define LOG_TRACE 1

void execute(vm_t * vm, int log_flag);
const cell * exec_step(vm_t * vm, FILE * fp, const cell * c_ptr);
//void execute_debug(vm_t * vm, FILE * breakpoint_fp,int * breakflag);

#endif /* __DBERGV_EXEC__ */