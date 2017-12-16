#ifndef __DBERGV_EXEC__
#define __DBERGV_EXEC__

#define NO_LOG 0
#define LOG_TRACE 1

void decompile(vm_t * vm);
void execute(vm_t * vm, int log_flag);
int execute_debug(vm_t * vm, FILE * breakpoint_fp);

#endif /* __DBERGV_EXEC__ */