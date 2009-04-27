#ifndef _ALTITUDE_SEXP_H
#define _ALTITUDE_SEXP_H


/* 
 * S-Expressions in Altitude
 * C code is loaded into the VM in an s-expression based format. This
 * file and sexp.c define parsing and manipulation functions for
 * Altitude's sexps.
 *
 * The format isn't quite standard Lisp-like sexps. A sexp in Altitude
 * looks like this (in approximate EBNF): 
 *  
 * sexp := '('
 *              (<location> <ws>)?
 *              <tag>
 *              (<ws> (<sexp> | <string> | <int>))* 
 *          <ws>? ')'
 * location := '@' <string> ':' <int> ':' <int>
 * int := [0-9]+ [a decimal non-negative integer, fits in a 64-bit unsigned int]
 * string := a double-quoted string, where backslash is the escape character
 *           and double-quote and backslash are escaped.
 * ws := any amount of newline, carriage return, tab or space
 * tag := one of the tags defined below
 * 
 * So, a sexp is an opening parenthesis, followed by optional location
 * information, followed by a tag, followed by any number of children
 * which are each integers, quoted strings or themselves sexps,
 * followed by a closing parenthesis.
 * 
 * Location information is of the form:
 *     @"somefile.c":20:500 
 * which means character 500 which is on line 20 of the file
 * "somefile.c". It is optional.
 */

/* Filenames are stored as atoms, makes everything less painful */
#include "atom.h"

/* for location information structures */
#include "bytecode.h"

/* The possible tags */
/* FIXME: very incomplete */
typedef enum {
  /* types & declarations */
  S_PROGRAM, S_STRUCT, S_UNION, S_TYPE, S_FIELDS, S_FIELD, S_COMPTYPE,
  S_INT, S_VAR,


  /* functions & code */
  S_FUNCTION, S_FORMALS, S_LOCALS, S_BODY,

  /* instructions */
  S_PLUS_INT, S_PLUS_SHORT, 
  S_MINUS_INT, 
  S_LT_INT,

  S_DEREF, S_ASSIGN, S_INDEX, S_OFFSET, 

  S_LOAD_L, S_LOAD_F, S_LOAD_G, S_CONSTANT_INT,
  S_CALL, S_CALLASSIGN, S_RETNONE, S_RETURN, 

  /* control flow */
  S_GOTO, S_CONDGOTO, S_LABEL,

} sexp_tag;

/* Get the string representation of a tag */
const char* sexp_tag_to_string(sexp_tag);


struct sexp;
typedef enum {ST_SEXP, ST_INTEGER, ST_STRING} sexp_elem_type;
/* An element of a sexp - either a string, an integer or an
   s-expression */
struct sexp_element{
  sexp_elem_type type;
  union sexp_child_data{
    struct sexp* sexp;
    int integer;
    atom string;
  } data;
};
struct sexp{
  /* tag */
  sexp_tag tag;

  struct location location;

  /* number of children */
  int nelems;
  /* array of children */
  struct sexp_element* elems;
};


inline static int is_sexp_with_tag(struct sexp_element s, sexp_tag t){
  return s.type == ST_SEXP && s.data.sexp->tag == t;
}


/* Parse a string into a newly-allocated sexp */
struct sexp* sexp_parse(char*);

/* Free a sexp and all of its children */
void sexp_free(struct sexp*);

/* Dump a sexp to stdout. For debugging */
void sexp_dump(struct sexp*);

#endif
