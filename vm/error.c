#include "error.h"
#include "stdio.h"
#include "stdlib.h"

void vm_err_warn(const char* cause){
  fprintf(stderr, "Warning: error in VM layer\n");
  fprintf(stderr, "%s%n", cause);
}

void vm_err_exit(const char* cause){
  fprintf(stderr, "Fatal: error in VM layer\n");
  fprintf(stderr, "%s%n", cause);
  exit(-1); 
//this doesn't really mean anything, unless someone's checking exit values
}
