#include <stdlib.h>
#include "callstack.h"
#include "memtypes.h"

#define DEFAULT_INVOKED_SIZE 20

struct stackframe* stackframe_new(struct stackframe* caller, struct function* callee){
  struct stackframe* frame = malloc(sizeof(struct stackframe));


  frame->func = callee;
  frame->parent = caller;
  frame->parent_inv_idx = 0;

  frame->locals = malloc(sizeof(struct blob*) * (callee->nformals + callee->nlocals));
  int local_idx = 0;
  for (int i=0;i<callee->nformals;i++){
    frame->locals[local_idx++] = blob_alloc(user_sizeof(callee->formals[i].type), callee->formals[i].type);
  }
  for (int i=0;i<callee->nlocals;i++){
    frame->locals[local_idx++] = blob_alloc(user_sizeof(callee->locals[i].type), callee->locals[i].type);
  }

  frame->invoked = malloc(sizeof(struct stackframe*) * DEFAULT_INVOKED_SIZE);
  frame->invoked_pos = 0;
  frame->invoked_cap = DEFAULT_INVOKED_SIZE;
  
  if (caller){
    //if this is not "int main()"
    frame->parent_inv_idx = caller->invoked_pos;
    caller->invoked[caller->invoked_pos++] = frame;
    if (caller->invoked_pos >= caller->invoked_cap){
      caller->invoked_cap *= 2;
      caller->invoked = realloc(caller->invoked, caller->invoked_cap * sizeof(struct stackframe*));
    }
  }
  return frame;
}


void stackframe_free(struct stackframe* f){
  
}
