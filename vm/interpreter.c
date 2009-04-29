#include "interpreter.h"


int32_t run(struct program* prog){
  struct vm_state vm;
  vm.program = prog;
  vm.frame = stackframe_new(NULL, prog->main_function);
  struct blob* mainret = blob_alloc(user_sizeof(PS_INT), PS_INT);
  vm.frame->wants_return = 1;
  vm.frame->return_ptr = pointer_to_blob(mainret, PS_INT);
  vm.now = 0;

  /* program counter */
  instruction* pc = vm.frame->func->code;

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

#define HAS_TYPE(type1, result)			                \
  do{								\
    if (stack[stacktop-1].type != type1{			\
      say(STACKTYPE, "Bad type in stack operation");		\
      stack[stacktop-1].valid = 0;				\
    }else{							\
      stack[stacktop-1].valid =	1;                              \
    }								\
    stack[stacktop-1].type = result;				\
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
  
#define UNOP(type1, op, result)				        \
  HAS_TYPE(type1, result);				        \
  stack[stacktop-1] = op stack[stacktop-1];	                \
  stacktop--;							\
  break;  

  //FIXME: arithmetic error checking (overflow and such)
  while (1){
    switch(instr_opcode(*pc)){
    case ARITH_PLUS: NEEDS_STACK(2);
      switch (instr_type(*pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,+,PS_INT);
      case PU_INT:; //FIXME: implement all of these :D
      case PS_SHORT: BINOP(PS_SHORT,PS_SHORT,+,PS_SHORT);
      case PU_SHORT:;
      case PS_CHAR:;
      case PU_CHAR:;
      case PS_LONG_LONG:;
      case PU_LONG_LONG:;
      default:
	say(INSTR, "complete addition not implemented");
	goto abnormal_quit;
      }
      break;
      
    case ARITH_MINUS: NEEDS_STACK(2);
      switch (instr_type(*pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,-,PS_INT);
      case PU_INT:;
      case PS_SHORT:;
      case PU_SHORT:;
      case PS_CHAR:;
      case PU_CHAR:;
      case PS_LONG_LONG:;
      case PU_LONG_LONG:;
      default:
	say(INSTR, "complete subtraction not implemented");
	goto abnormal_quit;
      }
      break;
      
    case ARITH_TIMES: NEEDS_STACK(2);
      switch (instr_type(*pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,*,PS_INT);
      case PU_INT:;
      case PS_SHORT:;
      case PU_SHORT:;
      case PS_CHAR:;
      case PU_CHAR:;
      case PS_LONG_LONG:;
      case PU_LONG_LONG:;
      default:
	say(INSTR, "complete multiplication not implemented");
	goto abnormal_quit;
      }
      break;
    
    case ARITH_DIV: NEEDS_STACK(2);
      switch (instr_type(*pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,/,PS_INT);
      case PU_INT:;
      case PS_SHORT:;
      case PU_SHORT:;
      case PS_CHAR:;
      case PU_CHAR:;
      case PS_LONG_LONG:;
      case PU_LONG_LONG:;
      default:
	say(INSTR, "complete division not implemented");
	goto abnormal_quit;
      }
      break;

    case ARITH_MOD: NEEDS_STACK(2);
      switch (instr_type(*pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,%,PS_INT);
      case PU_INT:;
      case PS_SHORT:;
      case PU_SHORT:;
      case PS_CHAR:;
      case PU_CHAR:;
      case PS_LONG_LONG:;
      case PU_LONG_LONG:;
      default:
	say(INSTR, "complete modulus not implemented");
	goto abnormal_quit;
      }
      break;

    case ARITH_NEGATE: NEEDS_STACK(1);
      switch (instr_type(*pc)){
      case PS_INT: UNOP(PS_INT,!,PS_INT);
      case PU_INT:;
      case PS_SHORT:;
      case PU_SHORT:;
      case PS_CHAR:;
      case PU_CHAR:;
      case PS_LONG_LONG:;
      case PU_LONG_LONG:;
      default:
	say(INSTR, "complete negation not implemented");
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
     
    case REL_GT: NEEDS_STACK(2);
      switch (instr_type(*pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,>,PS_INT);
      default:
	say(INSTR, "complete GT not implemented");
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
	if (varidx < 0 || varidx >= vm.frame->func->nvars){
	  sayf(VARACCESS, "local %d is out-of-bounds of function %s",
	       varidx, vm.frame->func->name->string);
	  goto abnormal_quit;
	}
	stack[stacktop].ptr = 
	  pointer_to_blob(vm.frame.locals[varidx],
			  vm.frame.func->vars[varidx].type);
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
	stack[stacktop].ptr = pointer_to_function(fidx);
	stack[stacktop].type = PT_PTR;
        stack[stacktop].valid = 1;
	stacktop++;
      }
      break;
    case VAR_LOAD_CONSTANT:
      {
        pc++;
        int cidx = (int)*pc;
        if (varidx < 0 || varidx >= vm.frame->func->nconsts){
          sayf(VARACCESS, "const %d is out-of-bounds of function %s",
               cidx, vm.frame->func->name->string);
          goto abnormal_quit;
        }
        stack[stacktop++] = vm.frame->func->consts[cidx];
      }
      break;
    case FUNC_CALL:
      {
        pc++;
        int nargs = (int)*pc;
        NEEDS_STACK(nargs + 2);//nargs args, 1 function, 1 retval-ptr
        //FIXME: moar typechecking
        struct function* func = vm.program->
          functions[pointer_deref_function(stack[stacktop-2].ptr)];
        vm.frame = stackframe_new(vm.frame, func);
        vm.frame->wants_return = 1;
        vm.frame->return_ptr = stack[stacktop-1].value.ptr
        vm.frame->return_addr = pc;
        stacktop -= 2;
        //copy in args
        if (nargs != vm.frame->func->nformals){
          say(INSTR, "wrong number of arguments passed to function");
          goto abnormal_quit;
        }
        if (stacktop != nargs){
          say(STACK, "stack not empty after function call");
          goto abnormal_quit;
        }

        for (int i=0;i<nargs;i++){
          pointer_assign(pointer_to_blob(vm.frame->locals[nargs-i],
                                         vm.frame->func->formals[nargs-i]->type),
                         stack[--stacktop]);
        }

        pc = vm.frame->func->code;
      }
      break;
    case FUNC_CALL_NONE:
      {
        pc++;
        int nargs = (int)*pc;
        NEED_STACK(nargs + 1);
        //FIXME: types
        struct function* func = vm.program->
          functions[pointer_deref_function(stack[stacktop-1].ptr)];
        vm.frame = stackframe_new(vm.frame,func);
        vm.frame->wants_return = 0;
        vm.frame->return_addr = pc;
        //copy in args
        if (nargs != vm.frame->func->nformals){
          say(INSTR, "wrong number of arguments passed to function");
          goto abnormal_quit;
        }
        if (stacktop != nargs){
          say(STACK, "stack not empty after function call");
          goto abnormal_quit;
        }

        for (int i=0;i<nargs;i++){
          pointer_assign(pointer_to_blob(vm.frame->locals[nargs-i],
                                         vm.frame->func->formals[nargs-i]->type),
                         stack[--stacktop]);
        }

        pc = vm.frame->func->code;
      }
      break;
    case FUNC_RETURN: NEED_STACK(1);
      {
        if (vm.frame->wants_return){
          pointer_assign(vm.frame->return_ptr, stack[stacktop-1]);
          stacktop--;
        }
        if (stacktop != 0){
          say(STACK, "stack not empty upon return");
          goto abnormal_quit;
        }
        pc = vm.frame->return_addr;
        vm.frame = vm.frame->parent;
        if (!vm.frame)goto normal_quit; //main returned
      }
      break;
    case FUNC_RETURN_NONE:
      {
        if (vm.frame->wants_return){
          say(INSTR, "function failed to return a value");
          goto abnormal_quit;
        }
        if (stacktop != 0){
          say(STACK, "stack not empty upon return");
          goto abnormal_quit;
        }
        pc = vm.frame->return_addr;
        vm.frame = vm.frame->parent;
        if (!vm.frame)goto normal_quit;
      }
      break;
    case GOTO_COND:NEED_STACK(1);
      if (!stack[--stacktop]){
        break;
      }
      //otherwise, fall through to GOTO_ALWAYS logic
    case GOTO_ALWAYS:
      {
        int offset = (int)*(pc+1);
        pc += offset;
        pc--; //to counter pc++ below.
      }
      break;
    }
    pc++;
  }

 normal_quit:
  say(DEBUG, "program ended");
  primitive_val retval = pointer_deref(pointer_to_blob(mainret, PS_INT));
  if (!retval.valid){
    say(INSTR, "main returned invalid value");
  }
  return USERDATA_PART(retval.value, PS_INT)

 abnormal_quit:
  say(INSTR, "abnormal quit");
  return -1;
}
