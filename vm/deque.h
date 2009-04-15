#ifndef _ALTITUDE_DEQUE_H
#define _ALTITUDE_DEQUE_H

#ifndef _ALTITUDE_CONTAINER_H
#include "container.h"
#endif

typedef _container* deque_t;

deque_t make_deque(size_t, size_t);
void destroy_deque(deque_t);

void deque_push_back(deque_t, void*);
void deque_push_front(deque_t, void*);

void* deque_peek_back(deque_t);
void* deque_peek_front(deque_t);

void* deque_pop_back(deque_t);
void* deque_pop_front(deque_t);
#endif
