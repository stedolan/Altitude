#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "sexp.h"
#include "error.h"


/* The string representation of the tags */
const char* sexp_names[]={
  [S_STRUCT] = "struct",
  [S_UNION] = "union",
  [S_LOOP] = "loop",
};
/* The number of tags, or, the length of sexp_names */
static const int NTAGS = sizeof(sexp_names)/sizeof(sexp_names[0]);

const char* sexp_tag_to_string(sexp_tag t){
  return sexp_names[t];
}

typedef static struct{
  atom fname;
  int lpos;
  int cpos;
  sexp_tag tag;
} intermediate;

static char* ltrim(char* str){
  char* end = str;
  while(*end != 0){
    if(isspace(*end)){
      end += 1;
    }else break;
  }
  return end;
}

static void dump_intermediate(intermediate i){
  printf("%s%n", "dumping sexp info: ");
  printf("%s%n", i.fname.string);
  printf("%s%n", itoa(i.lpos));
  printf("%s%n", itoa(i.cpos));
  printf("%s%n", sexp_tag_to_string(i.tag));
}

static intermediate parse_first_bit(char* str){
  assert(*str == '@');
  str += 2; //eat the @ and the "
  char* fname = strtok(str, "\"");
  atom at = atom_get(buf);
  
  str += (strlen(fname) + 2);
  char* lpos = strtok(str, ':');
  str += (strlen(lpos) + 1);
  char* cpos = strtok(str, ':');
  str += (strlen(cpos) + 1);
  char* tag = strtok(str, " \t\n\r(");
  
  intermediate i;
  i.fname = at;
  i.lpos = atoi(lpos);
  i.cpos = atoi(cpos);
  i.tag = atoi(tag);
  
  return i;
  }else return 0;
}

static 
 /*
  * A few assumptions: 
  * It's ok to return 0 when we run out of string to process
  * A full sexp is passed to this function (we don't run out of close-parens)
  * return 0 on all errors
  */
struct sexp* sexp_parse(char* str){
	
}

/* assumes sexp_parse mallocs each sexp and string individually */
void sexp_free(struct sexp* x){
  /* free children */
  for (int i=0; i<x->nelems; i++){
    switch (x->elems[i].type){
    case ST_SEXP:
      sexp_free(x->elems[i].data.sexp);
      break;
    case ST_STRING:
      free(x->elems[i].data.string);
      break;
    case ST_INTEGER:
      /* needs no freeing */
      break;
    }
  }
  /* possibly free filename (depends on reference count) */
  atom_decref(x->filename);
  /* free this node */
  free(x);
}

/* FIXME: incomplete */
void sexp_dump(struct sexp* x){
}
