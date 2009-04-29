#ifndef _ALTITUDE_MEMTYPES_H
#define _ALTITUDE_MEMTYPES_H


/* This is quite a complex file.
 * This contains most of the definitions for the Altitude VM's
 * internal structures, and the memory model.
 *
 */


#include <stdint.h>
#include "bitops.h"
#include "atom.h"
#include "sexp.h"


/* Userdata
 * All user-space data is stored in userdata objects, which are 64-bit
 * unions of either 8 chars, 4 shorts, 2 ints, a 64-bit int or a
 * pointer.
 */

/* number of primitive data (non-pointer) types */
#define N_PRIMITIVE_DATA_TYPES 9

typedef enum{
#define prim(name,reptype,id,cname) name = id,
#include "primitives.h"
#undef prim
} primtype;

/* A pointer in userspace. Not represented as a pointer in Altitude,
   but as a 64-bit key in Altitude's internal pointer-table */
typedef uint64_t userptr_t;

typedef union {
#define prim(name, reptype, id, cname) \
  reptype __prim_##name[sizeof(uint64_t)/sizeof(reptype)];
#include "primitives.h"
#undef prim
} userdata;

#define USERSIZE_USERDATA sizeof(uint64_t)


//To get or set a userdata, use USERDATA_PART
//e.g. printf("%d\n", USERDATA_PART(userdata, PS_INT));
//     USERDATA_PART(userdata,PS_INT) = 5;
#define USERDATA_PART(ud, p) ud.__prim_##p[0]


#define prim(name, reptype, id, cname) \
  typedef reptype __reptype_##name;
#include "primitives.h"
#undef prim

//Representation types for a user primitive
//e.g. REPTYPE(PS_INT) x = USERDATA_PART(u1, PS_INT);
//     USERDATA_PART(u2,PS_INT) = x;
#define REPTYPE(p) __reptype_##p

//To stop GCC warning about files not using these:
#ifdef __GNUC__
#define IGNORABLE __attribute__((unused))
#else
#define IGNORABLE
#endif


static IGNORABLE const char* __cnames[] = {
#define prim(name, reptype, id, cname) [name] = cname,
#include "primitives.h"
#undef prim
};
static IGNORABLE const char* __pnames[] = {
#define prim(name, reptype, id, cname) [name] = #name,
#include "primitives.h"
#undef prim
};
static IGNORABLE const int __usersz[] = {
#define prim(name, reptype, id, cname) [name] = \
  sizeof(reptype)/sizeof(REPTYPE(PS_CHAR)),
#include "primitives.h"
#undef prim
};

//The name of a primitive type (as a string)
//in either C (e.g. "signed int") or Altitude
//(e.g. "PS_INT")
#define PRIM_C_NAME(p) __cnames[p]
#define PRIM_ALT_NAME(p) __pnames[p]
//The size of a primitive type, as visible to a user
#define PRIM_USERSIZE(p) __usersz[p]

/* A userdata, tagged with information about what it 
   contains */
typedef struct{
  userdata value;
  primtype type;
  int valid;
} primitive_val;


/* Types
 * At compile-time, the typemap is built for the program. It contains
 * the memory layout of every data structure used in the program.
 */
typedef uint32_t usertype_t;

/* A size_t in userspace. Always 32-bit, whatever the architecture
   Altitude is running on. */
typedef uint32_t usersize_t;




/* size of a user type */
usersize_t user_sizeof(usertype_t);

/* base name (after stripping off N layers of pointers) */
atom get_base_type_name(usertype_t);

static inline int get_ptr_levels(usertype_t type){
  return (type >> 24) & 0xff;
}
static inline int is_ptr_type(usertype_t type){
  return get_ptr_levels(type) > 0;
}



usertype_t type_from_sexp(struct sexp*);
void build_typemap(int ntypes, struct sexp** types);

int get_type_offset(atom base, atom field);





/* A blob is a contiguous chunk of user-memory, used for both stack
   and heap allocations */
struct blob{
  /* length, in bytes.
   * This field is always a multiple of sizeof(userdata) except for
   * arrays of primitive types.
   */
  usersize_t length;
  
  /* data array. Actual user data is stored here. */
  userdata* data;
  
  /* the type stored at top level in this blob */
  usertype_t top_type;
  /* Map showing whether each element of data[] has a well-defined
     value */
  bitset_t valid_map;

  /* Pointer to the start of the blob */
  /* Has type "void*" for heap allocs */
  userptr_t pointer;
  usertype_t decltype;
  

  uint32_t flags;
};

struct blob* blob_alloc(usersize_t length, usertype_t type);



/* Pointer manipulation */

/* returns the type of the object pointed to by this pointer. */
usertype_t pointer_type(userptr_t);

/* returns a pointer to the start of a given blob */
userptr_t pointer_to_blob(struct blob*, usertype_t type);

/* returns a pointer to the function given by fidx */
userptr_t pointer_to_function(int fidx);

/* These functions return a true value if successful.
   If unsuccessful, the program should exit. */

/* index into an array */
int pointer_index(userptr_t*, userptr_t, int index);

/* point to a member of a struct or union */
int pointer_offset(userptr_t*, userptr_t, int field);

/* cast a pointer to a pointer of another type */
int pointer_cast(userptr_t*, userptr_t, usertype_t newtype);

/* return what's pointed to by a given pointer. Pointer must point to
   a primitive */
int pointer_deref(primitive_val*, userptr_t);

/* return what's pointed to by a given function pointer */
int pointer_deref_function(int*, userptr_t);

/* put something in the space pointed to by a given pointer. Must be a
   primitive, and pointer must point to a primitive */
int pointer_assign(userptr_t, primitive_val);


#endif
