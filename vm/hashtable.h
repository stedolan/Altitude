#ifndef _ALTITUDE_HT_H
#define _ALTITUDE_HT_H
/* Hashtables */
/* Two hashtables are provided, 
   one mapping zero-terminated strings to void* [ht_str_t],
   and one mapping uint64_t to void* [ht_int_t]
*/

/* for uint64_t */
#include <stdint.h>

typedef struct ht_str *ht_str_t;

/* Allocate/free memory for hashtable */
struct ht_str* ht_str_alloc();
void ht_str_free(struct ht_str*);

/* get and set an element.
   setting an element to NULL is the same as deleting it.
   there is no distinction between an element being NULL and the corresponding key being absent */
void* ht_str_get(struct ht_str*, const char*);
void ht_str_set(struct ht_str*, const char*, void*);
/* equivalent to set(NULL) */
void ht_str_del(struct ht_str*, const char*);


/* Iteration over the elements (keys and values) */

/* Iteration is read-only. Whenever the hashtable is modified, all iterators become invalid */

/* Iterator type. FIXME: need not be int. Whatever's easiest. Make it small */
typedef struct{ht_str_t ht; int idx;} ht_str_iter;

ht_str_iter ht_str_begin(ht_str_t);
/* Get the key or value at a particular place */
const char* ht_str_getkey(ht_str_iter);
void* ht_str_getvalue(ht_str_iter);
/* Returns true if this iterator is still iterating (i.e. getkey/getvalue will return something useful, it's not past-the-end) */
int ht_str_hasmore(ht_str_iter);
/* Advances the iterator */
ht_str_iter ht_str_next(ht_str_iter);

/* This all means that the following code would print out the contents of a hashtable, in some undefined order
 * for (ht_str_iter i = ht_str_begin(ht); ht_str_hasmore(i); i = ht_str_next(i)){
 *   printf("%s: %u\n", ht_str_getkey(i), ht_str_getvalue(i));
 * }
 */





/* Integer hashtable */

typedef struct ht_int *ht_int_t;
/* Allocate/free */
struct ht_int* ht_int_alloc();
void ht_int_free(struct ht_int*);

/* get, set, delete. Same semantics as string version */
void* ht_int_get(struct ht_int*, uint64_t);
void ht_int_set(struct ht_int*, uint64_t, void*);
void ht_int_del(struct ht_str*, uint64_t);

/* Iteration over the elements (keys and values). Semantics are the same as ht_str */
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
