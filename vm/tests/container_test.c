#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../vector.h"
#include "../deque.h"

/*
  Tests for vector and deque (and therefore container)
*/

int main(){
  int i, j;

  vector_t v = make_vector(10, 5);

  for(i = 0; i < 15; i++){
    vector_push_back(v, strdup("test"));
  }
  
  vector_set(v, 14, strdup("testing"));

  for(i = 0; i < v->location; i++){
    printf("%d\n", i);
    printf("%s\n", (char*)vector_get(v, i));
  }

  destroy_vector(v);

  deque_t d = make_deque(10, 5);

  for(i = 0; i < 10; i++){
    deque_push_back(d, strdup("back"));
    deque_push_back(d, strdup("front"));    
  }

  while(d->location){
    printf("%s\n", (char*)deque_peek_front(d));
    deque_pop_front(d);
  }

  destroy_deque(d);
  return 0;
}
