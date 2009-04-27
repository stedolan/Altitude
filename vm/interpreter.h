#ifndef _ALTITUDE_INTERPRETER_H
#define _ALTITUDE_INTERPRETER_H

#include <stdint.h>

#include "bytecode.h"
#include "memtypes.h"
#include "callstack.h"



struct vm_state{
  /* the non-changing parts: function definitions, types, etc. */
  struct program* prog;
  
  /* The current stack frame */
  struct stackframe* frame;
  
  uint64_t now; //"time": the number of instructions executed.
};


void run(struct program* prog);

#endif
