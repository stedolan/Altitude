#ifndef _ALTITUDE_VECTOR_H
#define _ALTITUDE_VECTOR_H

#include "stdlib.h"
#include "stdio.h"
#ifndef _ALTITUDE_ERROR_H
#include "error.h"
#endif

typedef struct{
	int location;
	size_t size;
	size_t threshold;
	size_t expansion;
	void** data;
} awesome_vector;

/*
	vector that can grow/shrink as needed
	location is the current last element in the vector
	size is the current size including empty elements,
	threshold is how close to size location is allowed to come before (de)allocating more memory
	expansion is how much we expand or contract the vector by each time
	data is a pointer to the start of the block of memory containing the vector.
*/

typedef awesome_vector* vector_t;
//a typedef to hide the fact that pointers are being thrown around

vector_t make_vector(size_t size, size_t threshold){
	vector_t s = malloc(sizeof(awesome_vector));
	s->location = 0;
	s->size = size;
	s->threshold = threshold;
	s->expansion = threshold-1; //gotta be less than threshold or we lose data contracting
	s->data = malloc(size*sizeof(void*));
	return s;
}
//allocate and set up the vector, returning a handle to it

void destroy_vector(vector_t s){
	free(s->data);
	free(s);
}

void push_back(vector_t s, void* pointer){
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
	if the vector is too small, we allocate it's size + the expansion factor
	then copy the old vector data into the new space, deallocate the old data
	then overwrite the old pointer with the new one.
	also change the current size to reflect the change, and finally insert the new member.
*/

void* pop_back(vector_t s){
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
  If the vector is too large, we do the opposite of what we did in push(), above.
*/

void set(vector_t s, int pos, void* pointer){
  if(pos >= location || pos < 0){
    vector_err_warn("index out of bounds in vector set()");//unhelpful, but rushing!
  }else{
    s->data[pos] = pointer;
  }
}

void* get(vector_t s, int pos){
  if(pos >= location || pos < 0){
    vector_err_warn("index out of bounds in vector set()");//unhelpful, but rushing!
    return 0;
  }else{
    return s->data[pos];
  }
}
#endif
