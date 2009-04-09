#include <stdio.h>
#include "../sexp.h"
int main(){
  char* sexp_str = 
    "(@\"main.c\":20:50 struct\n"
    "  (union)\n"
    "  23\n"
    "  \"a string with a backslash (\\\\) and a quote (\\\"). woo\" \n"
    "  (@\"main.c\":21:52 struct (struct) 43))\n"
    "(struct \"something else\")\n";
  struct sexp* s = sexp_parse(sexp_str);
  printf("Original:\n%s\n", sexp_str);
  printf("Parsed and dumped: \n");
  sexp_dump(s);
  sexp_free(s);
  return 0;
}
