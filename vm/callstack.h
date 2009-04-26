#ifndef _ALTITUDE_CALLSTACK_H
#define _ALTITUDE_CALLSTACK_H

#include "memtypes.h"
#include "bytecode.h"


struct stackframe{
  struct function* func;
  struct blob** locals;
  
  struct stackframe** invoked;
  int invoked_pos; //The next function called by this function will be the invoked_pos one to be called
  int invoked_cap; //the invoked array can store invoked_cap elements before being resized
};

struct stackframe* stackframe_new(struct stackframe* caller, struct function* callee);

#endif