#include <stdio.h>
#include <stdlib.h>

#include "sexp.h"
#include "bytecode.h"

#define MAXFILE (1024*1024)

int main(int argc, char** argv){
  if (argc <= 1){
    fprintf(stderr, "usage: %s <program>, where <program> is a sexp-compiled C file\n", argv[0]);
    exit(1);
  }
  FILE* prog = fopen(argv[1], "r");
  if (!prog){
    perror("Couldn't open program");
  }
  char progbuf[MAXFILE];
  fread(progbuf, MAXFILE, 1, prog);
  if (!feof(prog)){
    fprintf(stderr, "Couldn't read all of %s", argv[1]);
    exit(1);
  }
  struct sexp* sexpcode = sexp_parse(progbuf);
  if (!sexpcode){
    fprintf(stderr, "Couldn't parse s-expressions in %s", argv[1]);
    exit(1);
  }
  sexp_dump(sexpcode);
  sexp_free(sexpcode);
}
