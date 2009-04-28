#ifndef _ALTITUDE_ERROR_H
#define _ALTITUDE_ERROR_H
#include <stdio.h>
#include "atom.h"




/* location information */
/* If there is no location information for this sexp, filename is
 * NULL and line and bytepos are -1.
 */
struct location{
  atom filename;
  int line, bytepos;
};

/* printing locations
   Usage: printf("%s happened here: " LOC_FMT ", %d times.",
                 whathappened, LOC_ARGS(location), ntimes);
*/

#define LOC_FMT "@\"%s\":%d:%d"
#define LOC_ARGS(l) l.filename->string, l.line, l.bytepos



/* There are a number of categories of diagnostic.
 * Each has a 3-digit numeric code.
 * The first digit gives a overall category (debug, internal
 * consistency checking, errors, warnings, etc) and the next two
 * digits give a more specific error message.
 */
typedef enum{
  /* 000-099
     debugging information for Altitude itself */
  E_GENERIC_DEBUG = 0,
  E_DEBUG, //debugging info
  E_WARNING, //something odd happened inside Altitude
  E_ASSERT, //Assertion. These may not be checked in optimised builds.
  E_ERROR, //internal consistency check, if one of these is printed bad
           //things have happened
  
  //internal compiler errors - due to a bug in Altitude
  E_SEXP, //s-expression parsing
  E_COMPILE, //s-expression compilation to bytecode
  E_TYPEMAP, //building typemap


  //VM errors, should all be impossible due to compilation process
  //checking them ahead-of-time
  E_INSTR, //illegal instruction executed
  E_STACK, //stack underflow in vm
  E_STACKTYPE, //bad types on stack in vm
  E_VARACCESS, //variable access

  /* 100-199
     User warnings: Altitude has detected that the user's code did
     something it probably didn't mean to. It is possible that these
     are flagged in error (the user might intentionally do strange
     stuff). */
  E_GENERIC_USERWARN = 100,
  E_ALLOC_UNDEF, //allocating an amount of memory given by a garbage value
  
  /* 200-299
     User errors. The user did something which was definitely wrong
     (i.e. may cause a segfault or other crash in a real
     implementation, or provably corrupts data) */
  E_GENERIC_USERERR = 200,
  E_NULL_DEREF, //dereference a null pointer
  
  
} message_category;


/* Examples of use:
   say(DEBUG, "this isn't working");
   sayf(ERROR, "%d should be %d, but isn't", someval, someotherval);
   sayf(NULL_DEREF, "null pointer id #%d was dereferenced", ptr->id);

   Internal debugging information is printed with a file and line number.
*/
#define sayf(category, format, ...)			\
  do{							\
    if (_should_say(E_##category)){			\
      if ((E_##category) < 100){			\
	fprintf(stderr, "[%03d] %s:%d: " format "\n",	\
		(E_##category), __FILE__, __LINE__,	\
		__VA_ARGS__);				\
      }else{						\
	fprintf(stderr, "[%03d] " format "\n",		\
		(E_##category), __VA_ARGS__);		\
      }							\
      _possibly_quit(E_##category);			\
    }							\
  }while(0)

#define say(category, mesg) sayf(category,"%s",mesg)

//Internal functions, do not call directly
int _should_say(message_category);
void _possibly_quit(message_category);

#endif
