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


//assert a string
static void p_litstr(char** pos, const char* str){
  if (!*pos)return;
  char* p = *pos;
  int len = strlen(str);
  if (!strncmp(p, str, len)){
    *pos = p + len;
  }else{
    *pos = NULL;
  }
}

//parse a string, return as an atom
//Strings are C-style double-quoted strings
static atom p_string(char** pos){
  p_litstr(pos, "\"");
  if (!*pos) return NULL;
  char* p = *pos;
  int capacity = 2000;
  char* buf = malloc(capacity);
  int bufpos = 0;
  while (*p && *p != '"'){
    if (bufpos >= capacity){
      capacity *= 2;
      buf = realloc(buf, capacity);
    }
    char c = *p;
    if (c == '\\'){
      p++;
      switch(*p){
      case '\\':c='\\';break;
      case '"':c='"'; break;
      case 'n':c='\n';break;
      case 'r':c='\r';break;
      case 't':c='\t';break;
      case 'f':c='\f';break;
      case '0':c='\0';break;
      default:c=*p;break; // ignore backslashes when unknown, so "\j" = "j"
      }
    }
    buf[bufpos++] = c;
    p++;
  }
  *pos = p;
  p_litstr(pos, "\"");
  if (!*pos) return NULL;
  else return atom_get_len(buf, bufpos);
}

//parse an integer
static int p_int(char** pos){
  if (!*pos)return -1;
  char* int_end;
  long int ret = strtol(*pos, &int_end, 10);
  if (int_end == *pos){
    //no characters matched, error.
    *pos = NULL;
    return -1;
  }
  *pos = int_end;
  return (int)ret;
}

//skip whitespace
static void p_ws(char** pos){
  if (!*pos)return;
  while (isspace(**pos))
    (*pos)++;
}

//parse location information
static struct location p_location(char** pos){
  struct location l = {.filename = NULL, .bytepos = -1, .line = -1};
  if (!*pos)return l;
  p_litstr(pos, "@");
  l.filename = p_string(pos);
  p_litstr(pos, ":");
  l.line = p_int(pos);
  p_litstr(pos, ":");
  l.bytepos = p_int(pos);
  return l;
}

static struct sexp* p_sexp(char** pos);

//parse a single sexp-element
static struct sexp_element p_sexp_element(char** pos){
  struct sexp_element ret;
  if (!*pos)return ret;
  char next = **pos;
  if (next == '('){
    ret.type = ST_SEXP;
    ret.data.sexp = p_sexp(pos);
  }else if (next == '"'){
    ret.type = ST_STRING;
    ret.data.string = p_string(pos);
  }else if (isdigit(next) || next == '+' || next == '-'){
    ret.type = ST_INTEGER;
    ret.data.integer = p_int(pos);
  }else{
    //error
    *pos = NULL;
  }
  return ret;
}

static struct sexp* p_sexp(char** pos){
  if (!*pos)return NULL;
  struct sexp* ret = malloc(sizeof(struct sexp));
  p_litstr(pos, "(");
  p_ws(pos);
  if (*pos && **pos == '@'){
    ret->location = p_location(pos);
  }else{
    ret->location.filename = NULL;
    ret->location.line = -1;
    ret->location.bytepos = -1;
  }
  p_ws(pos);
  if (!*pos){
    free(ret);
    return NULL;
  }

  //Find the tag
  int tag = -1;
  for (int i=0;i<NTAGS;i++){
    if (!strncmp(*pos, sexp_names[i], strlen(sexp_names[i]))){
      tag = i;
      break;
    }
  }
  if (tag == -1){
    free(ret);
    return NULL;
  }
  ret->tag = (sexp_tag) tag;
  *pos += strlen(sexp_names[tag]);
  //Parse the subelements
  int capacity = 10;
  struct sexp_element* elems = calloc(capacity, sizeof(struct sexp_element));
  int size = 0;
  p_ws(pos);
  while (*pos && **pos != ')'){//while not error and not end of sexp...
    elems[size++] = p_sexp_element(pos);
    if (size == capacity){
      capacity *= 2;
      elems = realloc(elems, capacity);
    }
    p_ws(pos);
  }
  p_litstr(pos, ")");
  ret->nelems = size;
  ret->elems = elems;
  return ret;
}

//Fill in location information for sexps that have none
//NB: some sexps might have filenames but no line numbers
static void fill_in_location(struct sexp* s){
  struct location* default_loc = &s->location;
  for (int i=0;i<s->nelems;i++){
    if (s->elems[i].type == ST_SEXP){
      struct location* loc = &s->elems[i].data.sexp->location;
      if (loc->filename == NULL){
	loc->filename = atom_addref(default_loc->filename);
      }
      if (loc->line == -1){
	loc->line = default_loc->line;
	loc->bytepos = default_loc->bytepos;
      }
      fill_in_location(s->elems[i].data.sexp);
    }
  }
}

struct sexp* sexp_parse(char* s){
  struct sexp* ret = p_sexp(&s);
  if (!ret){
    printf("Invalid sexp\n");
    return NULL;
  }
  if (!ret->location.filename){
    ret->location.filename = atom_get("[unknown file]");
    ret->location.line=0;
    ret->location.bytepos=0;
  }
  fill_in_location(ret);
  return ret;
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
      atom_decref(x->elems[i].data.string);
      break;
    case ST_INTEGER:
      /* needs no freeing */
      break;
    }
  }
  free(x->elems);
  /* possibly free filename (depends on reference count) */
  atom_decref(x->location.filename);
  /* free this node */
  free(x);
}

/* Dumps a sexp out to stdout.
   Not quite the same as the input format. Just for debugging */
void sexp_dump(struct sexp* x){
  printf("[<%s>:%d:%d %s%s\n", x->location.filename->string, x->location.line, x->location.bytepos, sexp_tag_to_string(x->tag), x->nelems == 0 ? "]": "");
  if (x->nelems > 0){
    for (int i=0;i<x->nelems;i++){
      struct sexp_element* e = &x->elems[i];
      if (e->type == ST_SEXP)sexp_dump(e->data.sexp);
      else if (e->type == ST_INTEGER)printf("%d\n",e->data.integer);
      else if (e->type == ST_STRING)printf("{%s}\n", e->data.string->string);
    }
    printf("]\n");
  }
}
