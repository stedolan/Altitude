#include "callstack.h"
#include "memtypes.h"

#define DEFAULT_INVOKED_SIZE 20

void stackframe_new(struct stackframe* caller, struct function* callee){
  struct stackframe* frame = malloc(sizeof(struct stackframe));


  frame->func = callee;
  frame->parent = caller;

  frame->locals = malloc(sizeof(struct blob*) * (callee->nformals + callee->nlocals));
  int local_idx = 0;
  for (int i=0;i<callee->nformals;i++){
    frame->locals[local_idx++] = blob_alloc(user_sizeof(callee->formal_types[i]));
  }
  for (int i=0;i<callee->nlocals;i++){
    frame->locals[local_idx++] = blob_alloc(user_sizeof(callee->local_types[i]));
  }

  frame->invoked = malloc(sizeof(struct stackframe*) * DEFAULT_INVOKED_SIZE);
  frame->invoked_pos = 0;
  frame->invoked_cap = DEFAULT_INVOKED_SIZE;
  
  if (caller){
    //if this is not "int main()"
    caller->invoked_pos++;
    if (caller->invoked_pos >= caller->invoked_cap){
      caller->invoked_cap *= 2;
      caller->invoked = realloc(caller->invoked, caller->invoked_cap * sizeof(struct stackframe*));
    }
    caller->invoked[caller->invoked_pos] = frame;
  }
  return frame;
}
