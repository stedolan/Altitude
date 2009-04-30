#include "interpreter.h"
#include "user_stdlib.h"
#include <assert.h>

static struct vm_state vm;

struct location current_location(){
  if (!vm.frame)return unknown_location();
  struct location loc;
  loc.function = vm.frame->func;
  
  //binary search for current location
  int pc = vm.pc - loc.function->code;
  if (!(pc >= 0 && pc < loc.function->codelen))
    return unknown_location(); //message during function call
  int* positions = loc.function->locpos;
  struct location* locs = loc.function->locs;


  int i = 0, j = loc.function->nlocs;
  while (j - i > 1){
    int newpos = (i+j)/2;
    if (positions[newpos] <= pc)
      i = newpos;
    else
      j = newpos;
  }
  
  if (j > i){
    if (j < loc.function->nlocs)
      return locs[j];
    else
      return locs[i];
  }else{
    return unknown_location();
  }
}


REPTYPE(PS_INT) run(struct program* prog){
  vm.program = prog;
  vm.now = 0;
  if (!prog->main_function){
    say(NO_MAIN, "No main function found");
    goto abnormal_quit;
  }
  vm.frame = stackframe_new(NULL, prog->main_function);
  struct blob* mainret = blob_alloc(user_sizeof(PS_INT), PS_INT);
  vm.frame->wants_return = 1;
  vm.frame->return_ptr = pointer_to_blob(mainret, PS_INT);
  vm.pc = vm.frame->func->code;


  primitive_val stack[1000];
  /* the position of the next stack element to be added, or 
     the number of items in the stack
     stack[stacktop-1] is the last element pushed onto the stack */
  int stacktop = 0; 

#define TRY(x) do{if(!(x))goto abnormal_quit;}while(0)

#ifndef ALTITUDE_OPTIMISED
#define NEEDS_STACK(n)						\
  do{								\
    if (stacktop < (n)){                                        \
      sayf(STACK, "Stack underflow, need %d places, have %d",	\
	   (n), stacktop);					\
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
    if (stack[stacktop-1].type != type1){			\
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
#else
#define NEEDS_STACK(x) 
#define HAS_TYPES_2(t1,t2,r)
#define HAS_TYPE(t,r)
#define REQUIRES_POINTER
#endif

#define BINOP(type1, type2, op, result)                         \
  HAS_TYPES_2(type1, type2, result);                            \
  USERDATA_PART(stack[stacktop-2].value,result) =               \
    USERDATA_PART(stack[stacktop-2].value,type1)                \
           op                                                   \
    USERDATA_PART(stack[stacktop-1].value,type2);               \
  stacktop--;                                                   \
  break;
  
#define UNOP(type1, op, result)				        \
  HAS_TYPE(type1, result);				        \
  USERDATA_PART(stack[stacktop-1].value,result)                 \
    = op USERDATA_PART(stack[stacktop-1].value,type1);          \
  stacktop--;							\
  break;  


  say(PROGRAM_START, "Program starting");
  //FIXME: arithmetic error checking (overflow and such)
  while (1){
    sayf(TRACE, "Executing %s", opcode_name(instr_opcode(*vm.pc)));
    switch(instr_opcode(*vm.pc)){
    case ARITH_PLUS: NEEDS_STACK(2);
      switch (instr_type(*vm.pc)){
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
      switch (instr_type(*vm.pc)){
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
      switch (instr_type(*vm.pc)){
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
      switch (instr_type(*vm.pc)){
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
      switch (instr_type(*vm.pc)){
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
      switch (instr_type(*vm.pc)){
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
      switch (instr_type(*vm.pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,<,PS_INT);
      default:
	say(INSTR, " rest of LT not implemented");
	goto abnormal_quit;
      }
      break;
     
    case REL_GT: NEEDS_STACK(2);
      switch (instr_type(*vm.pc)){
      case PS_INT: BINOP(PS_INT,PS_INT,>,PS_INT);
      default:
	say(INSTR, "complete GT not implemented");
	goto abnormal_quit;
      }
      break;

    case PTR_DEREF: NEEDS_STACK(1); REQUIRES_POINTER;
      TRY(pointer_deref(&stack[stacktop-1], USERDATA_PART(stack[stacktop-1].value, PT_PTR)));
      break;
    case PTR_ASSIGN: NEEDS_STACK(2);
      stacktop--;
      REQUIRES_POINTER;
      stacktop++;
      TRY(pointer_assign(USERDATA_PART(stack[stacktop-2].value,PT_PTR), stack[stacktop-1]));
      stacktop-=2;
      break;

    /* FIXME: are indices really ints?? */
    case PTR_INDEX: NEEDS_STACK(2); HAS_TYPES_2(PS_INT, PT_PTR, PT_PTR);
      TRY(pointer_index(&USERDATA_PART(stack[stacktop-2].value,PT_PTR),
                        USERDATA_PART(stack[stacktop-2].value,PT_PTR),
                        (int)USERDATA_PART(stack[stacktop-1].value,PS_INT)));
      stacktop--;
      break;
    case PTR_OFFSET: NEEDS_STACK(1); REQUIRES_POINTER;
      /* takes an immediate, the offset */
      vm.pc++;
      TRY(pointer_offset(&USERDATA_PART(stack[stacktop-1].value,PT_PTR), 
                         USERDATA_PART(stack[stacktop-1].value,PT_PTR), (int)*vm.pc));
      break;
    case PTR_CAST: NEEDS_STACK(1); REQUIRES_POINTER;
      {
        usertype_t type = 0;
        vm.pc++;
        type |= (unsigned_immediate)*vm.pc;
        type <<= 16;
        vm.pc++;
        type |= (unsigned_immediate)*vm.pc;
        TRY(pointer_cast(&USERDATA_PART(stack[stacktop-1].value,PT_PTR),
                         USERDATA_PART(stack[stacktop-1].value,PT_PTR),
                         type));
      }
      break;
    case VAR_LOAD_LOCAL:
      /* takes an immediate, the var */
      {
	vm.pc++;
	int varidx = (int)*vm.pc;
	if (varidx < 0 || varidx >= vm.frame->func->nvars){
	  sayf(VARACCESS, "local %d is out-of-bounds of function %s",
	       varidx, vm.frame->func->name->string);
	  goto abnormal_quit;
	}
	USERDATA_PART(stack[stacktop].value,PT_PTR) = 
	  pointer_to_blob(vm.frame->locals[varidx],
			  vm.frame->func->vars[varidx].type);
	stack[stacktop].valid = 1;
	stack[stacktop].type = PT_PTR;
	stacktop++;
      }
      break;
    case VAR_LOAD_FUNC:
      {
	vm.pc++;
	int fidx = (int)(signed_immediate)*vm.pc;
	if (fidx < -user_stdlib_count || fidx >= vm.program->nfunctions){
	  sayf(VARACCESS, "function %d does not exist",
	       fidx);
	  goto abnormal_quit;
	}
	USERDATA_PART(stack[stacktop].value,PT_PTR) = pointer_to_function(fidx);
	stack[stacktop].type = PT_PTR;
        stack[stacktop].valid = 1;
	stacktop++;
      }
      break;
    case VAR_LOAD_CONSTANT:
      {
        vm.pc++;
        int cidx = (int)*vm.pc;
        if (cidx < 0 || cidx >= vm.frame->func->nconsts){
          sayf(VARACCESS, "const %d is out-of-bounds of function %s",
               cidx, vm.frame->func->name->string);
          goto abnormal_quit;
        }
        stack[stacktop++] = vm.frame->func->consts[cidx];
      }
      break;
    case FUNC_CALL:
    case FUNC_CALL_NONE:
      {
        int wants_return = (instr_opcode(*vm.pc) == FUNC_CALL) ? 1 : 0;
        vm.pc++;
        int nargs = (int)*vm.pc;
        NEEDS_STACK(nargs + wants_return ? 2 : 1);//nargs args, 1 function, 0-1 retval-ptr
        //FIXME: moar typechecking
        int fidx;
        TRY(pointer_deref_function(&fidx, USERDATA_PART(stack[stacktop-1].value,PT_PTR)));
        stacktop--; //pop function addr
        if (stacktop != nargs + (wants_return ? 1 : 0)){
          say(STACK, "stack not empty after function call");
          goto abnormal_quit;
        }
        if (fidx >= 0){
          //user function
          struct function* func = &vm.program->functions[fidx];
          vm.frame = stackframe_new(vm.frame, func);
          vm.frame->wants_return = wants_return;
          if (wants_return)
            vm.frame->return_ptr = USERDATA_PART(stack[stacktop-nargs-1].value,PT_PTR);
          vm.frame->return_addr = vm.pc;
          //copy in args
          if (nargs != vm.frame->func->nformals){
            say(INSTR, "wrong number of arguments passed to function");
            goto abnormal_quit;
          }

          
          for (int i=0;i<nargs;i++){
            TRY(pointer_assign(pointer_to_blob(vm.frame->locals[nargs-i-1],
                                               vm.frame->func->formals[nargs-i-1].type),
                               stack[--stacktop]));
          }
          if (wants_return)
            stacktop --;//pop off return ptr
          assert(stacktop == 0);
          vm.pc = vm.frame->func->code - 1;//-1 to handle the vm.pc++ below
        }else{
          //syscall
          fidx = -fidx - 1;
          assert(fidx >= 0 && fidx < user_stdlib_count);
          system_function syscall = user_stdlib_funcs[fidx];
          userptr_t retptr = 0;
          if (wants_return) retptr = USERDATA_PART(stack[stacktop - nargs - 1].value,PT_PTR);
          TRY((*syscall)(wants_return, retptr, nargs, stack + wants_return));
          stacktop = 0;
        }

      }
      break;
    case FUNC_RETURN: NEEDS_STACK(1);
      {
        if (vm.frame->wants_return){
          TRY(pointer_assign(vm.frame->return_ptr, stack[stacktop-1]));
          stacktop--;
        }
        if (stacktop != 0){
          say(STACK, "stack not empty upon return");
          goto abnormal_quit;
        }
        vm.pc = vm.frame->return_addr;
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
        vm.pc = vm.frame->return_addr;
        vm.frame = vm.frame->parent;
        if (!vm.frame)goto normal_quit;
      }
      break;
    case GOTO_COND:NEEDS_STACK(1);
      //All logic (boolean &&, etc) in Altitude uses PS_INT
      if (!USERDATA_PART(stack[--stacktop].value,PS_INT)){
        vm.pc++;//eat the immediate so we don't try to execute it
        break;
      }
      //otherwise, fall through to GOTO_ALWAYS logic
    case GOTO_ALWAYS:
      {
        int offset = (int)*((signed_immediate*)(vm.pc+1));
        vm.pc += offset;
        vm.pc--; //to counter vm.pc++ below.
      }
      break;
    default:
      say(INSTR, "Illegal instruction executed");
      goto abnormal_quit;
    }
    vm.pc++;
    vm.now++;
  }

 normal_quit:
  sayf(PROGRAM_END_OK, "Program ended after %llu instructions executed", vm.now);
  primitive_val retval;
  TRY(pointer_deref(&retval, pointer_to_blob(mainret, PS_INT)));
  if (!retval.valid){
    say(INSTR, "main returned invalid value");
  }
  REPTYPE(PS_INT) mainval =  USERDATA_PART(retval.value, PS_INT);
  sayf(PROGRAM_END_OK, "Function main returned %d", (int)mainval);
  return mainval;

 abnormal_quit:
  sayf(PROGRAM_END_CRASH, "Program exited abnormally after %llu instructions executed", vm.now);
  return -1;
}
