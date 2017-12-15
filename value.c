#include "value.h"
#include <assert.h>
#include <stdio.h>

#define NUM_BITS 16
#define MAX_VAL 32767 // highest constant value
#define MAX_REG 32775 // highest value corresponding to a register

value_t * Value_set_bit(value_t * v, int idx){
	assert(v);
	assert(idx <= NUM_BITS);

	(*v) |= (1 << idx);

	return v;
}

value_t * Value_clear_bit(value_t * v, int idx){
	assert(v);
	assert(idx <= NUM_BITS);

	(*v) &= ~(1 << idx);

	return v;
}

value_t * Value_flip_bit(value_t * v, int idx){
	assert(v);
	assert(idx <= NUM_BITS);

	(*v) ^= (1 << idx);

	return v;
}

int Value_test_bit(value_t * v, int idx){
	assert(v);
	assert(idx <= NUM_BITS);

	return ((*v) & (1 << idx));
}

value_t Value_bitwise_not(value_t v){
	/*
	for(int i = 0; i < 15; i++){
		Value_flip_bit(v,i); // inefficient!
	}*/

	return v ^ MAX_VAL;
}

size_t Value_get_register_idx(const value_t * v){
	assert(v);
	assert((*v) > MAX_VAL);
	assert((*v) <= MAX_REG);
	size_t ret = (size_t) ((*v - MAX_VAL) - 1);
	//if(ret == 7) printf("MAGIC REGISTER!!!!\n");

	return ret;
}

size_t Value_is_reg(const value_t * v){
	assert(v);
	return ((((*v) > MAX_VAL) && ((*v) <= MAX_REG)) ? 1 : 0);
}