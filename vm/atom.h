#ifndef _ALTITUDE_ATOM_H
#define _ALTITUDE_ATOM_H

#include <stdint.h>

/*
 * An atom is a reference-counted zero-terminated string. For any
 * possible string, there will be at most one atom with that
 * value. This means that atoms can successfully be compared with
 * "==". For those who know such things, this is similar to a Lisp or
 * Scheme symbol.
 */

 /* Atoms carry a unique 64-bit ID and a 32-bit precomputed hash */
typedef struct __atom{
  char* string;
  int length;
  uint64_t id;
  uint32_t hashcode;

  int refcnt;
  struct __atom* next; /* used for internal hashtable */
} *atom;

//Get a zero-terminated string
atom atom_get(const char* string);
//Get a string with explicit length
//Needed for strings with NULLs in them
atom atom_get_len(const char* string, int len);
atom atom_addref(atom);
void atom_decref(atom);
#endif
