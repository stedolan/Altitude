#ifndef _ALTITUDE_STACK_H
#define _ALTITUDE_STACK_H
#include "stdlib.h"
#include "stdio.h"

typedef struct{
	int location;
	size_t size;
	size_t threshold;
	size_t expansion;
	void** data;
} awesome_stack;

/*
	stack that can grow/shrink as needed
	location is which element will be returned by the next pop()
	size is the current size including empty elements,
	threshold is how close to size location is allowed to come before (de)allocating more memory
	expansion is how much we expand or contract the stack by each time
	data is a pointer to the start of the block of memory containing the stack.
*/

typedef awesome_stack* stack_t;
//a typedef to hide the fact that pointers are being thrown around

stack_t make_stack(size_t size, size_t threshold){
	stack_t s = malloc(sizeof(awesome_stack));
	s->location = 0;
	s->size = size;
	s->threshold = threshold;
	s->expansion = threshold-1; //gotta be less than threshold or we lose data contracting
	s->data = malloc(size*sizeof(void*));
	return s;
}

//allocate and set up the stack, returning a handle to it

void destroy_stack(stack_t s){
	free(s->data);
	free(s);
}

void push(stack_t s, void* pointer){
	if((s->size - s->location) <= s->threshold){
		void* temp = malloc(sizeof(void*) * (s->size + s->expansion));
		memcpy(temp, s->data, s->size+s->expansion);
		free(s->data);
		s->data = temp;
		s->size += s->expansion;
	}
	
	s->data[s->location] = pointer;
	s->location += 1; 
}

/*
	if the stack is too small, we allocate it's size + the expansion factor
	then copy the old stack data into the new space, deallocate the old data
	then overwrite the old pointer with the new one.
	also change the current size to reflect the change, and finally insert the new member.
*/

void* peek(stack_t s){
	return s->data[s->location-1];
}

void* pop(stack_t s){
	void* temp = s->data[s->location-1];
	s->location -= 1;
	
	if((s->size - s->location) >= s->threshold){
		void* temp = malloc(sizeof(void*) * (s->size - s->expansion));
		memcpy(temp, s->data, s->size-s->expansion);
		free(s->data);
		s->data = temp;
		s->size -= s->expansion;
	}
	
	return temp;
}

/*
  If the stack is too large, we do the opposite of what we did in push(), above.
*/
#endif
