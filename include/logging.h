//
// Created by David Bergvelt on 9/5/18.
//

#ifndef SYNACOR_LOGGING_H
#define SYNACOR_LOGGING_H

#include <vm.h>
#include <mem.h>

char * string_of_cell(vm_t * vm, const cell * inst_ptr, char * buf, int * pc_ptr);

void state_dump(vm_t * vm);

void state_dump_full(vm_t * vm);

void decompile(vm_t * vm);

#endif //SYNACOR_LOGGING_H
