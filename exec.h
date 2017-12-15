#ifndef __DBERGV_EXEC__
#define __DBERGV_EXEC__

#define NO_LOG 0
#define LOG_TRACE 1

void execute_with_trace(vm_t * vm);
void execute(vm_t * vm, int log_flag);

#endif /* __DBERGV_EXEC__ */