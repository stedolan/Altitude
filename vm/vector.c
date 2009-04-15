#ifndef _ALTITUDE_VECTOR_H
#include "vector.h"
#endif

vector_t make_vector(size_t size, size_t threshold){
	return make_container(size, threshold);
}

void destroy_vector(vector_t s){
  destroy_container(s);
}

void vector_push_back(vector_t s, void* pointer){
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

void vector_set(vector_t s, int pos, void* pointer){
  if(pos >= s->location || pos < 0){
 	//FIXME:do some warning here
  }else{
    free(s->data[pos]);
    s->data[pos] = pointer;
  }
}

void* vector_get(vector_t s, int pos){
  if(pos >= s->location || pos < 0){
  	//FIXME:do some warning here
    return 0;
  }else{
    return s->data[pos];
  }
}
