#ifndef _ALTITUDE_HT_H
#define _ALTITUDE_HT_H
/* Hashtables */
/* Two hashtables are provided, 
   one mapping atoms (zero-terminated strings, see atom.h) to void* [ht_atom_t],
   and one mapping uint64_t to void* [ht_int_t]
*/

/* for uint64_t */
#include <stdint.h>
#include "atom.h"

typedef struct ht_atom *ht_atom_t;

/* Allocate/free memory for hashtable */
ht_atom_t ht_atom_alloc();
void ht_atom_free(ht_atom_t);

/* get and set an element.
   setting an element to NULL is the same as deleting it.
   there is no distinction between an element being NULL and the corresponding key being absent */
void* ht_atom_get(ht_atom_t, atom);
void ht_atom_set(ht_atom_t, atom, void*);
/* equivalent to set(NULL) */
void ht_atom_del(ht_atom_t, atom);


/* Iteration over the elements (keys and values) */

/* Iteration is read-only. Whenever the hashtable is modified, all iterators become invalid */

/* Iterator type. FIXME: need not be int. Whatever's easiest. Make it small */
typedef struct{ht_atom_t ht; int idx;} ht_atom_iter;

ht_atom_iter ht_atom_begin(ht_atom_t);
/* Get the key or value at a particular place */
atom ht_atom_getkey(ht_atom_iter);
void* ht_atom_getvalue(ht_atom_iter);
/* Returns true if this iterator is still iterating (i.e. getkey/getvalue will return something useful, it's not past-the-end) */
int ht_atom_hasmore(ht_atom_iter);
/* Advances the iterator */
ht_atom_iter ht_atom_next(ht_atom_iter);

/* This all means that the following code would print out the contents of a hashtable, in some undefined order
 * for (ht_atom_iter i = ht_atom_begin(ht); ht_atom_hasmore(i); i = ht_atom_next(i)){
 *   printf("%s: %u\n", ht_atom_getkey(i), ht_atom_getvalue(i));
 * }
 */





/* Integer hashtable */

typedef struct ht_int *ht_int_t;
/* Allocate/free */
ht_int_t ht_int_alloc();
void ht_int_free(ht_int_t);

/* get, set, delete. Same semantics as string version */
void* ht_int_get(ht_int_t, uint64_t);
void ht_int_set(ht_int_t, uint64_t, void*);
void ht_int_del(ht_int_t, uint64_t);

/* Iteration over the elements (keys and values). Semantics are the same as ht_atom */
typedef struct{ht_int_t ht; int idx;} ht_int_iter;

ht_int_iter ht_int_begin(ht_int_t);
/* Get the key or value at a particular place */
const char* ht_int_getkey(ht_int_iter);
void* ht_int_getvalue(ht_int_iter);
/* Returns true if this iterator is still iterating (i.e. getkey/getvalue will return something useful, it's not past-the-end) */
int ht_int_hasmore(ht_int_iter);
/* Advances the iterator */
ht_int_iter ht_int_next(ht_int_iter);

/* This all means that the following code would print out the contents of a hashtable, in some undefined order
 * for (ht_int_iter i = ht_int_begin(ht); ht_int_hasmore(i); i = ht_int_next(i)){
 *   printf("%s: %u\n", ht_int_getkey(i), ht_int_getvalue(i));
 * }
 */


#endif
