#include <stdlib.h>
#include <stdio.h>
#include "sexp.h"


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

/* FIXME: incomplete */
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
