#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sexp.h"
#include "bytecode.h"
#include "interpreter.h"
#include "uiproto.h"
#define MAXFILE (1024*1024)

int main(int argc, char** argv){
  if (argc <= 1){
    fprintf(stderr, "usage: %s [run|dump] <program>, where <program> is a sexp-compiled C file\n", argv[0]);
    exit(1);
  }
  char* file = argv[argc-1];
  FILE* prog = fopen(file, "r");
  if (!prog){
    perror("Couldn't open program");
  }
  char progbuf[MAXFILE];
  fread(progbuf, MAXFILE, 1, prog);
  if (!feof(prog)){
    fprintf(stderr, "Couldn't read all of %s", file);
    exit(1);
  }
  struct sexp* sexpcode = sexp_parse(progbuf);
  if (!sexpcode){
    fprintf(stderr, "Couldn't parse s-expressions in %s", file);
    exit(1);
  }
  struct program* program = compile(sexpcode);
  sexp_free(sexpcode);

  if (!program){
    fprintf(stderr, "Couldn't compile\n");
    exit(1);
  }

  if (argc == 3){
    if (!strcmp(argv[1], "run")){
      run(program);
    }else if (!strcmp(argv[1], "dump")){
      program_dump(program);
    }
  }else{ 
    while(1){
      get_command();
      if(parse_command(program)){
        break;
      }
    }
  }
  return 0;
}
