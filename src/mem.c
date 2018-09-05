#include "../include/mem.h"
#include "../include/value.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 10
#define MAX_SIZE 32768 // given 15 bit addresses

#define max(a,b) (((a) > (b)) ? (a) : (b))

Memory *Memory_create() {

  Memory * ret = malloc(sizeof(Memory));
  if(!ret){
    fprintf(stderr, "Malloc failed!\n");
    exit(1);
  }

  ret->array = malloc(INITIAL_CAPACITY*sizeof(cell*)); 
  ret->size = 0;
  ret->capacity = INITIAL_CAPACITY;
  
  return ret;
}

void Memory_destroy(Memory *memory) {
  assert(memory);
  size_t i;
  for(i = 0; i < (memory->size); i++){
    if((memory->array)[i]) free((memory->array)[i]);
  }
  free(memory->array);
  free(memory);
}

size_t Memory_size(Memory *memory) {
  assert(memory);
  return memory->size;
}

void Memory_resize(Memory *memory, size_t new_size) {
  assert(memory);
  assert(new_size > 0);
  assert(new_size < MAX_SIZE);

  // CASE ONE: Same size
  if(new_size == memory->size){
    return; // nothing to be done
  }

  // CASE TWO: New size less than orig size and capacity
  size_t old_capacity = memory->capacity;
  if((new_size < memory->size) && (new_size < old_capacity)){
    size_t i;
    for(i = new_size; i < memory->size; i++){
      if((memory->array)[i]) free((memory->array)[i]); // free data outside of size range
    }
    memory->size = new_size;
    return;
  }
  // CASE THREE: New size greater than old size but less or equal to old capacity
  else if((new_size > memory->size) && (new_size <= memory->capacity)){
    size_t i;
    for(i = memory->size; i < new_size; i++) (memory->array)[i] = NULL; // NULL padding
    memory->size = new_size;
    return;
  }

  // CASE FOUR: New size greater than old size and old capacity
  else if(new_size > old_capacity){
    size_t new_capacity = max(old_capacity * 2, new_size);
    cell ** new = malloc(new_capacity * sizeof(cell*)); //allocate new array
    if(!new){
      fprintf(stderr, "Malloc failed!\n");
      exit(1);
    }
    size_t i;
    for(i = memory->size; i < new_size; i++) new[i] = NULL; // NULL padding
    for(i = 0; i < memory->size; i++) new[i] = (memory->array)[i]; // copy pointers to data
    free(memory->array);    
    memory->capacity = new_capacity;
    memory->size = new_size;
    memory->array = new;
    return;
  }
}


void Memory_set(Memory *memory, size_t index, const cell *c) {
  assert(memory);
  assert(index >=  0);
  assert(index < memory->size);

  if((memory->array)[index]) free((memory->array)[index]);

  if(!c){
    (memory->array)[index] = NULL;
    return;
  }

  (memory->array)[index] = malloc(sizeof(cell));
  *((memory->array)[index]) = *c;
  //strcpy((memory->array)[index],str);
  return;
}

const cell *Memory_get(Memory *memory, size_t index) {
  assert(memory);
  assert(index >= 0);
  //assert(index < memory->size);
  if(!(index < memory->size)) return NULL;

  assert((memory->array)[index]);
  return (memory->array)[index];
}

void Memory_insert(Memory *memory, size_t index, const cell *c) {
  assert(memory);
  assert(index >= 0);
  
  size_t resizeTo;
  if(index < memory->size) resizeTo = (memory->size)+1; // calculate new memory size
  else resizeTo = index+1;

  Memory_resize(memory, resizeTo); // resize

  if(resizeTo == (index+1)){
    if(!c) (memory->array)[index] = NULL;
    else {
      (memory->array)[index] = malloc(sizeof(cell));
       *((memory->array)[index]) = *c;
      //strcpy((memory->array)[index],str); // if index is being inserted at index >= size, no shifting should need to take place
    }
    return;
  }

  // in the case we do need to shift:
  cell * temp = (memory->array)[index];
  cell * temp2;
  if(!c){
    (memory->array)[index] = NULL;
  }
  else{
    (memory->array)[index] = malloc(sizeof(cell));
     *((memory->array)[index]) = *c;
  }

  size_t i;
  for(i = index+1; i < memory->size; i++){
    temp2 = (memory->array)[i];
    (memory->array)[i] = temp;
    temp = temp2;
  }
  
  return;
}

void Memory_delete(Memory *memory, size_t index) {
  assert(memory);
  assert(index >= 0);
  assert(index < memory->size); // can't delete something outside of the memory

  if(index == ((memory->size)-1)){ // simplest case: no shifting needed.
      Memory_resize(memory, index); // just resize so the last element is deallocated
      return;
    }

  // otherwise, we need to manually dealloc and shift
  if((memory->array)[index]) free((memory->array)[index]);
  
  cell * temp;
  size_t i;
  for(i = index; i < ((memory->size)-1); i++){  // shifting
    temp = (memory->array)[i+1];
    (memory->array)[i] = temp;
  }
  (memory->array)[(memory->size)-1] = NULL; //remember to set last entry to NULL so we don't accidentally free data in resize()
  // finally, resize to correct size
  Memory_resize(memory, ((memory->size)-1));

  return;
}

void Memory_append(Memory *memory, const cell *c) {
  assert(memory);
  
  Memory_resize(memory, ((memory->size)+1)); // grow memory by 1
  
  if(!c){
      (memory->array)[(memory->size)-1] = NULL;
    }
  else{
    (memory->array)[(memory->size)-1] = malloc(sizeof(cell));
     *((memory->array)[memory->size-1]) = *c;
  }
  return;
}