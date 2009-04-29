#ifndef _ALTITUDE_INTERPRETER_H
#define _ALTITUDE_INTERPRETER_H

#include <stdint.h>

#include "bytecode.h"
#include "memtypes.h"
#include "callstack.h"



struct vm_state{
  /* the non-changing parts: function definitions, types, etc. */
  struct program* program;
  
  /* The current stack frame */
  struct stackframe* frame;
  
  /* The program counter */
  instruction* pc;
  
  uint64_t now; //"time": the number of instructions executed.
};


REPTYPE(PS_INT) run(struct program* prog);

struct location current_location();//for debugging purposes

#endif
