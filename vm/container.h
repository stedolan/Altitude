#ifndef _ALTITUDE_CONTAINER_H
#define _ALTITUDE_CONTAINER_H
#include <string.h>
#include <stdlib.h>

typedef struct{
	int location;
	size_t size;
	size_t threshold;
	size_t expansion;
	void** data;
} _container;

_container* make_container(size_t, size_t);
void destroy_container(_container*);
#endif
