#ifndef __DBERGV_MEMORY__
#define __DBERGV_MEMORY__
#include <stdlib.h>
#include "value.h"

typedef struct{
	uint16_t addr;
	uint16_t value;
} cell;

typedef struct {
  /**
   * An array of pointers to the entries in the array.
   * NULL values are allowed.
  */
  cell ** array;

  /* The current size of the 'array'. */
  size_t size;

  /* The allocated size of the 'array'. */
  size_t capacity;
} Memory;

/* Allocate and return a new Memory structure. */
Memory *Memory_create();

/**
 * Deallocate a Memory structure.
 * Every non-NULL entry in array from array[0] to array[size-1] will
 * need to be free()d.
*/
void Memory_destroy(Memory *memory);

/* Return the number of entries in use in this array. */
size_t Memory_size(Memory *memory);

/**
 * Sets the size of the array.
 *
 * If the array grows, new entries will be initialized to NULL.
 * If the array shrinks, entries past the new end of the array
 * will be deallocated.
 *
 * If the new size is larger than the capacity of the array, the
 * capacity will need to be increased, meaning the array will need to
 * be reallocated.  The new capacity should be at least twice as big
 * as it was before. In other words:
 *
 * if new_size > capacity:
 *      new_capacity = maximum(capacity * 2, new_size);
*/
void Memory_resize(Memory *memory, size_t new_size);

/**
 * Allocate a copy of 'c' and store it in array[index].  If 'cell' is NULL,
 * just store NULL in array[index].  Note that if the vector is allocating
 * memory for the entry (which will happen when th entry is not NULL), then it
 * should also be the freeing it.
*/
void Memory_set(Memory *memory, size_t index, const cell *cell);

/**
 * Returns the entry stored at array[index].
 * If array[index]==NULL, this will return NULL.
 *
 * This does not return a copy of the entry; it returns the actual
 * cell* stored at array[index]. This means the caller must not
 * deallocate the entry (if the entry is non-null).
*/
const cell *Memory_get(Memory *memory, size_t index);

/**
 * Inserts a copy of 'val' or NULL if val is NULL
 * and stores it in array[index].
 * Note the array[index] could already contain a entry,
 * in which case the entry at array[index] and all subsequent
 * entries should be shifted down.
 * Also note that index could be well beyond the position of
 * the last entry. In which case the vector should resize()
 * to barely accommodate that new entry.
*/
void Memory_insert(Memory *memory, size_t index, const cell *val);

/**
 * Removes whatever entry is contained in array[index].
 * Note that index does not have to be the last element of the vector,
 * in which case you should not only remove the entry at array[index],
 * but also shift all subsequent entries down.
*/
void Memory_delete(Memory *memory, size_t index);

/**
 * Inserts 'val' (or NULL) right after the last element in the vector.
*/
void Memory_append(Memory *memory, const cell *val);


#endif /* __DBERGV_MEMORY__ */