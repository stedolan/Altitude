#include "interpreter.h"


void run(struct program* prog){
  struct vm_state vm;
  vm.program = prog;
  vm.stackframe = stackframe_new(NULL, prog->main_function);
  vm.now = 0;

  /* program counter */
  instruction* pc = vm.stackframe->func->code;

  primitive_val stack[1000];
  /* the position of the next stack element to be added, or 
     the number of items in the stack
     stack[stacktop-1] is the last element pushed onto the stack */
  int stacktop = 0; 

#define NEEDS_STACK(n)						\
  do{								\
    if (stacktop < n){						\
      sayf(STACK, "Stack underflow, need %d places, have %d",	\
	   n, stacktop);					\
      goto abnormal_quit;					\
    }								\
  }while(0)

#define HAS_TYPES_2(type1, type2, result)			\
  do{								\
    if (stack[stacktop-1].type != type1 ||			\
	stack[stacktop-2].type != type2){			\
      say(STACKTYPE, "Bad types in stack operation");		\
      stack[stacktop-2].valid = 0;				\
    }else{							\
      stack[stacktop-2].valid =					\
      stack[stacktop-1].valid && stack[stacktop-2].valid;	\
    }								\
    stack[stacktop-2].type = result;				\
  }while(0)



#define REQUIRES_POINTER				\
  do{							\
    if (stack[stacktop-1].type != PT_PTR){		\
      say(STACKTYPE, "operation requires pointer");	\
      stack[stacktop-1].valid = 0;			\
    }							\
  }while(0)


#define BINOP(type1, type2, op, result)				\
  HAS_TYPES_2(type1, type2, result);				\
  stack[stacktop-2] = stack[stacktop-2] op stack[stacktop-1];	\
  stacktop--;							\
  break;

  //FIXME: arithmetic error checking (overflow and such)
  while (1){
    switch(instr_opcode(*pc)){
    case ARITH_PLUS: NEEDS_STACK(2);
      switch (instr_type(*pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,+,PS_INT);
      default:
	say(INSTR, " proper addition not implemented");
	goto abnormal_quit;
      }
      break;
    case REL_LT: NEEDS_STACK(2);
      switch (instr_type(*pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,<,PS_INT);
      default:
	say(INSTR, " rest of LT not implemented");
	goto abnormal_quit;
      }
      break;
     

    case PTR_DEREF: NEEDS_STACK(1); REQUIRES_POINTER;
      stack[stacktop-1] = pointer_deref(stack[stacktop-1]);
      break;
    case PTR_ASSIGN: NEEDS_STACK(2);
      stacktop--;
      REQUIRES_POINTER;
      stacktop++;
      pointer_assign(stack[stacktop-2].ptr, stack[stacktop-1]);
      stacktop-=2;
      break;

    /* FIXME: are indices really ints?? */
    case PTR_INDEX: NEEDS_STACK(2); HAS_TYPES_2(PS_INT, PT_PTR, PT_PTR);
      stack[stacktop-2] = pointer_index(stack[stacktop-2], stack[stacktop-1]);
      stacktop--;
      break;
    case PTR_OFFSET: NEEDS_STACK(1); REQUIRES_POINTER;
      /* takes an immediate, the offset */
      pc++;
      stack[stacktop-1] = pointer_offset(stack[stacktop-1], (int)*pc);
      break;
    case VAR_LOAD_LOCAL:
      /* takes an immediate, the var */
      {
	pc++;
	int varidx = (int)*pc;
	if (varidx < 0 || varidx >= vm.stackframe->func->nvars){
	  sayf(VARACCESS, "local %d is out-of-bounds of function %s",
	       varidx, func->name->string);
	  goto abnormal_quit;
	}
	stack[stacktop].ptr = 
	  pointer_to_blob(vm.stackframe.locals[varidx],
			  vm.stackframe.func->vars[varidx].type);
	stack[stacktop].valid = 1;
	stack[stacktop].type = PT_PTR;
	stacktop++;
      }
      break;
    case VAR_LOAD_FUNC:
      {
	pc++;
	int fidx = (int)*pc;
	if (fidx < 0 || fidx >= vm.program->nfunctions){
	  sayf(VARACCESS, "function %d does not exist",
	       fidx);
	  goto abnormal_quit;
	}
	stack[stacktop].ptr = vm.stackframe;//FIXME!!!
	stack[stacktop].type = PT_PTR;
	stacktop++;
      }
      break;
    case FUNC_CALL:
      //FIXME!!!
      break;
    case FUNC_RETURN:
      //FIXME!!!
      break;
    }
  }
}
