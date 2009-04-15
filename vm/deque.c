#ifndef _ALTITUDE_DEQUE_H
#include "deque.h"
#endif

deque_t make_deque(size_t size, size_t threshold){
	return make_container(size,threshold);
}

void destroy_deque(deque_t s){
  destroy_container(s);
}

void deque_push_back(deque_t s, void* pointer){
	if((s->size - s->location) <= s->threshold){
		void* temp = malloc(sizeof(void*) * (s->size + s->expansion));
		memcpy(temp, s->data, s->location*sizeof(void*));
		free(s->data);
		s->data = temp;
		s->size += s->expansion;
	}
	
	s->data[s->location] = pointer;
	s->location += 1; 
}

void deque_push_front(deque_t s, void* pointer){
	if((s->size - s->location) <= s->threshold){
		void* temp = malloc(sizeof(void*) * (s->size + s->expansion));
		memcpy(temp, s->data, s->location*sizeof(void*));
		free(s->data);
		s->data = temp;
		s->size += s->expansion;
	}
	
	memmove(s->data+1, s->data, sizeof(void*)*(s->location));
	s->data[0] = pointer;
	s->location += 1; 
}

/*
	if the deque is too small, we allocate it's size + the expansion factor
	then copy the old deque data into the new space, deallocate the old data
	then overwrite the old pointer with the new one.
	also change the current size to reflect the change, and finally insert the new member.
*/

void* deque_peek_back(deque_t s){
	return s->data[s->location-1];
}

void* deque_peek_front(deque_t s){
	return s->data[0];
}

void* deque_pop_back(deque_t s){
	void* temp = s->data[s->location-1];
	s->location -= 1;
	
	if((s->size - s->location) >= s->threshold){
		void* temp2 = malloc(sizeof(void*) * (s->size - s->expansion));
		memcpy(temp2, s->data, s->location*sizeof(void*));
		free(s->data);
		s->data = temp2;
		s->size -= s->expansion;
	}
	
	return temp;
}

void* deque_pop_front(deque_t s){
	void* temp = s->data[0];
	s->location -= 1;
		
	if((s->size - s->location) >= s->threshold){
		void* temp2 = malloc(sizeof(void*) * (s->size - s->expansion));
		memcpy(temp2, s->data+1, s->location*sizeof(void*)); //take the removed element into account
		free(s->data);
		s->data = temp2;
		s->size -= s->expansion;
	}else{
		memmove(s->data, s->data+1, s->location*sizeof(void*)); //take the removed element into account
	}
	
	return temp;
}

/*
  If the deque is too large, we do the opposite of what we did in push(), above.
*/
