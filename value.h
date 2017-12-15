#ifndef __DBERGV_VALUE__
#define __DBERGV_VALUE__
#include <stdlib.h>

typedef uint16_t value_t; //unsigned int value_t; 
/*struct value_s{
	int bv;
} value_t;*/

value_t * Value_set_bit(value_t * v, int idx);

value_t * Value_clear_bit(value_t * v, int idx);

value_t * Value_flip_bit(value_t * v, int idx);

int Value_test_bit(value_t * v, int idx);

value_t Value_bitwise_not(value_t v);

size_t Value_get_register_idx(const value_t * v);

size_t Value_is_reg(const value_t * v);

#endif /* __DBERGV_VALUE__ */