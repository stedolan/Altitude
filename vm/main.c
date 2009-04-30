#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sexp.h"
#include "bytecode.h"
#include "interpreter.h"

#define MAXFILE (1024*1024)

int main(int argc, char** argv){
  if (argc <= 2){
    fprintf(stderr, "usage: %s [run|dump] <program>, where <program> is a sexp-compiled C file\n", argv[0]);
    exit(1);
  }
  FILE* prog = fopen(argv[2], "r");
  if (!prog){
    perror("Couldn't open program");
  }
  char progbuf[MAXFILE];
  fread(progbuf, MAXFILE, 1, prog);
  if (!feof(prog)){
    fprintf(stderr, "Couldn't read all of %s", argv[2]);
    exit(1);
  }
  struct sexp* sexpcode = sexp_parse(progbuf);
  if (!sexpcode){
    fprintf(stderr, "Couldn't parse s-expressions in %s", argv[2]);
    exit(1);
  }
  struct program* program = compile(sexpcode);
  sexp_free(sexpcode);

  if (!program){
    fprintf(stderr, "Couldn't compile\n");
    exit(1);
  }
  if (!strcmp(argv[1], "run")){
    run(program);
  }else if (!strcmp(argv[1],"dump")){
    program_dump(program);
  }
  return 0;
}
