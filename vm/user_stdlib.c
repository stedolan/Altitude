#include "user_stdlib.h"
#include "error.h"

int sys_malloc(int wantreturn, userptr_t retval, 
               int nargs, primitive_val* args){
  if (nargs != 1){
    say(BAD_SYSCALL_ARGS, "malloc called with wrong number of arguments");
    return 0;
  }
  //FIXME: strictly speaking, malloc is called with a size_t, not an int
  //so the following is wrong
  if (args[0].type != PS_INT){
    say(BAD_SYSCALL_ARGS, "malloc called with non-int argument");
    return 0;
  }
  if (!args[0].valid){
    say(DUBIOUS_SYSCALL_INVOCATION, "argument to malloc appears invalid - unknown amount of memory allocated");
  }
  REPTYPE(PS_INT) bytes = USERDATA_PART(args[0].value, PS_INT);
  if (!wantreturn){
    sayf(DUBIOUS_SYSCALL_INVOCATION,
         "return value of malloc ignored - memory leak of %d bytes",
         (int)bytes);
    return 1;//this is OK, but stupid
  }else if (bytes < 0){
    say(BAD_SYSCALL_ARGS, "malloc called with negative argument");
    return 0;
  }else{
    sayf(DEBUG, "mallocing %d bytes", bytes);
    primitive_val newmem;
    newmem.valid = 1;
    USERDATA_PART(newmem.value,PT_PTR) = 
      pointer_to_blob(blob_alloc(bytes,0,NULL),0);
    newmem.type = PT_PTR;
    return pointer_assign(retval, newmem);
  }
}
int sys_putint(int wantreturn, userptr_t retval,
               int nargs, primitive_val* args){
  if (wantreturn){
    say(BAD_SYSCALL_ARGS, "putint does not return a value");
    return 0;
  }
  if (nargs != 1 || args[0].type != PS_INT){
    say(BAD_SYSCALL_ARGS, "putint passed bad args");
    return 0;
  }
  if (!args[0].valid){
    say(DUBIOUS_SYSCALL_INVOCATION, "puting passed possibly undefined value");
    return 0;
  }
  sayf(USER_OUTPUT, "User output: %d", USERDATA_PART(args[0].value, PS_INT));
  return 1;
}



const int user_stdlib_count = 1;
const char* user_stdlib_names[] = {"__altitude_malloc", "__altitude_putint"};
const system_function user_stdlib_funcs[] = {sys_malloc,sys_putint};
