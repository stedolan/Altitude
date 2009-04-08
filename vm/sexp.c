#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifndef _ALTITUDE_SEXP_H
#include "sexp.h"
#endif
#ifndef _ALTITUDE_ATOM_H
#include "atom.h"
#endif
#ifndef _ALTITUDE_ERROR_H
#include "error.h"
#endif


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
  int length; //the length of the location information string processed
} intermediate;

static char* ltrim(char* str, int* amt){
  while(*str != 0){
    if(isspace(*str)){
      str += 1;
      *amt +=1;
    }else break;
  }
  return str;
}

static void dump_intermediate(intermediate i){
  printf("%s%n", "dumping sexp info: ");
  printf("%s%n", i.fname.string);
  printf("%s%n", itoa(i.lpos));
  printf("%s%n", itoa(i.cpos));
  printf("%s%n", sexp_tag_to_string(i.tag));
  printf("%s%n", itoa(i.length));
}

static intermediate parse_first_bit(char* str){
  int size = 0;
  atom at;
  char* lpos = 0;
  char* cpos = 0;
  char* tag = 0;
  
  if(*str == '@'){
    str += 2; //eat the @ and the "
    size += 2;
    
    char* fname = strtok(str, "\"");
    at = atom_get(buf);
    str += (strlen(fname) + 2);
    size += (strlen(fname) + 2);

    lpos = strtok(str, ':');
    str += (strlen(lpos) + 1);
    size += (strlen(lpos) + 1);
  
    cpos = strtok(str, ':');
    str += (strlen(cpos) + 1);
    size += (strlen(cpos) + 1);
  }else{
    at = atom_get("");
    vm_err_warn("no location information in sexp");
    
    tag = strtok(str, " \t\n\r(");
    size += strlen(tag);
    
    if(strlen(tag) == 0){
      vm_err_warn("no sexp tag specified in expression");
    }
  }
    
  intermediate i;
  i.fname = at;
  i.lpos = atoi(lpos);
  i.cpos = atoi(cpos);
  i.tag = atoi(tag);
  i.length = size;
  return i;
}


 /*
  * A few assumptions: 
  * It's ok to return 0 when we run out of string to process
  * A full sexp is passed to this function (we don't run out of close-parens)
  * return 0 on all errors
  */
struct sexp* sexp_parse(char* str, int* datalength){
	if(*str == 0){
	  return 0;
	}else{
	  struct sexp* s = malloc(sizeof(struct sexp));
	  intermediate i = parse_first_bit(s);
	  int data_length = 0;
	  
	  str += i.length;
	  data_length += i.length;
	  
	  char* data = ltrim(str, &data_length); //get rid of optional whitespace
	  
    while(*str != 0 && *str != ')'){
      if(*data == '('){ //nested sexp case
        int len = 0;
        struct sexp* p = sexp_parse(data, &len);
        str += len+1;          //+1 to skip over the closing ')', which would
        data_length += len+1;  //incorrectly trigger the while condition.
      
        struct sexp_elem* se = malloc(sizeof(struct sexp_elem));
        se->type = sexp_elem_type[ST_SEXP];
        se->sexp_child_data = p;
        sexp_add_elem(se, s);
        
      }else if(isalpha(*data)){ //string case
        int len = 0;
        while(isalpha(*data)){
          len += 1;
          data += 1;
        }
        char* the_string = malloc(len);
        strncpy(the_string, data, len);
        data += len;
      }else if(isdigit(*data)){ //int case
        
      }else{
        vm_err_exit("unrecognised syntax found processing data in sexp");
      }
    }
	  return s;
	}
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
