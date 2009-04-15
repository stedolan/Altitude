#ifndef _ALTITUDE_VECTOR_H
#define _ALTITUDE_VECTOR_H

#ifndef _ALTITUDE_CONTAINER_H
#include "container.h"
#endif

typedef _container* vector_t;
//a typedef to hide the fact that pointers are being thrown around

vector_t make_vector(size_t, size_t);
//allocate and set up the vector, returning a handle to it

void destroy_vector(vector_t);

void vector_push_back(vector_t, void*);
void vector_set(vector_t, int, void*);
void* vector_get(vector_t, int);
#endif
