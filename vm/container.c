#ifndef _ALTITUDE_CONTAINER_H
#include "container.h"
#endif

_container* make_container(size_t size, size_t threshold){
	_container* s = malloc(sizeof(_container));
	s->location = 0;
	s->size = (size > 0 ? size : 1);
	s->threshold = (threshold > 1 ? threshold : 2);
	s->expansion = threshold-1; //gotta be less than threshold or we lose data contracting
	s->data = malloc(size*sizeof(void*));
	return s;
}

void destroy_container(_container* s){
	free(s->data);
	free(s);
}

