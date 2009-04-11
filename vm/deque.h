#ifndef _ALTITUDE_DEQUE_H
#define _ALTITUDE_DEQUE_H

typedef struct{
	int location;
	size_t size;
	size_t threshold;
	size_t expansion;
	void** data;
} awesome_deque;

/*
	deque that can grow/shrink as needed
	location is the current last element in the deque
	size is the current size including empty elements,
	threshold is how close to size location is allowed to come before (de)allocating more memory
	expansion is how much we expand or contract the deque by each time
	data is a pointer to the start of the block of memory containing the deque.
*/

typedef awesome_deque* deque_t;
//a typedef to hide the fact that pointers are being thrown around

deque_t make_deque(size_t size, size_t threshold){
	deque_t s = malloc(sizeof(awesome_deque));
	s->location = 0;
	s->size = size;
	s->threshold = threshold;
	s->expansion = threshold-1; //gotta be less than threshold or we lose data contracting
	s->data = malloc(size*sizeof(void*));
	return s;
}
//allocate and set up the deque, returning a handle to it

void destroy_deque(deque_t s){
	free(s->data);
	free(s);
}

void push_back(deque_t s, void* pointer){
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

void push_front(deque_t s, void* pointer){
	if((s->size - s->location) <= s->threshold){
		void* temp = malloc(sizeof(void*) * (s->size + s->expansion));
		memcpy(temp, s->data, s->size+s->expansion);
		free(s->data);
		s->data = temp;
		s->size += s->expansion;
	}
	
	memmove(s->data+(sizeof(void*)), s->data, sizeof(void*)*s->location);
	s->data[0] = pointer;
	s->location += 1; 
}

/*
	if the deque is too small, we allocate it's size + the expansion factor
	then copy the old deque data into the new space, deallocate the old data
	then overwrite the old pointer with the new one.
	also change the current size to reflect the change, and finally insert the new member.
*/

void* pop_back(deque_t s){
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

void* pop_front(deque_t s){
	void* temp = s->data[0];
		
	if((s->size - s->location-1) >= s->threshold){ //pretend we've removed an element, location-1
		void* temp = malloc(sizeof(void*) * (s->size - s->expansion));
		memcpy(temp, s->data, s->size-s->expansion);
		free(s->data);
		s->data = temp;
		s->size -= s->expansion;
	}
	
    memmove(s->data, s->data+(sizeof(void*)), sizeof(void*)*s->location);
	s->location -= 1;
	return temp;
}

/*
  If the deque is too large, we do the opposite of what we did in push(), above.
*/
#endif
