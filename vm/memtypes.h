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


/* Userdata
 * All user-space data is stored in userdata objects, which are 64-bit
 * unions of either 8 chars, 4 shorts, 2 ints, a 64-bit int or a
 * pointer.
 */

typedef enum{
  PS_CHAR, /* plain "char" in Altitude is signed char */
  PU_CHAR,
  PS_SHORT,
  PU_SHORT,
  //int and long are equivalent (both 32 bits)
  PS_INT,
  PU_INT,
  //long long is 64 bits
  PS_LONG_LONG,
  PU_LONG_LONG,



  PT_PTR = 100 /* must come last; it's a bit of a special case */
} primtype;

/* A pointer in userspace. Not represented as a pointer in Altitude,
   but as a 64-bit key in Altitude's internal pointer-table */
typedef uint64_t userptr_t;



#define USERDATA_CHAR_LEN sizeof(uint64_t)
#define USERDATA_SHORT_LEN sizeof(uint64_t) / sizeof(uint16_t)
#define USERDATA_INT_LEN sizeof(uint64_t) / sizeof(uint32_t)
#define USERDATA_LL_LEN 1
#define USERDATA_PTR_LEN 1

/* number of primitive data (i.e. non-pointer) types */
#define N_PRIMITIVE_DATA_TYPES 8

typedef union {
  int8_t s_char[USERDATA_CHAR_LEN];
  uint8_t u_char[USERDATA_CHAR_LEN];
  int16_t s_short[USERDATA_SHORT_LEN];
  uint16_t u_short[USERDATA_SHORT_LEN];
  int32_t s_int[USERDATA_INT_LEN];
  uint32_t u_int[USERDATA_INT_LEN];
  int64_t s_long_long[USERDATA_LL_LEN];
  uint64_t u_long_long[USERDATA_LL_LEN];
  
  userptr_t ptr[USERDATA_PTR_LEN];
} userdata;


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

static const int USER_SIZEOF_PTR = USERDATA_CHAR_LEN / USERDATA_PTR_LEN;



typedef enum{TF_STRUCT, TF_UNION, TF_ARRAY, TF_PRIMITIVE} type_flavour;
/* the layout of a single type */
struct type_layout{
  type_flavour flavour;
  usersize_t size;

  //possible values of the path of a pointer are 0 to nchildren
  int nchildren;
  usertype_t* children; //type of the object at each path
  uint32_t* bytepos; //byte offsets
  
};

/* the typemap for all types in the program */
struct typemap{
  int ntypes;
  /* The names are kept seperately to the layouts as multiple types
     can share a layout (i.e. typedefs) */
  atom* names; //names of the types
  struct type_layout* layouts; //an array which can be indexed by usertype_t
};



/* size of a user type */
usersize_t user_sizeof(usertype_t);

/* base name (after stripping off N layers of pointers) */
atom get_base_type_name(usertype_t);

struct type_layout* get_type_layout(usertype_t type);

static inline int get_ptr_levels(usertype_t type){
  return (type >> 24) & 0xff;
}
static inline int is_ptr_type(usertype_t type){
  return get_ptr_levels(type) > 0;
}






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
  bitset_t undef_map;

  uint32_t flags;
};

struct blob* blob_alloc(usersize_t length);






struct pointer{
  struct blob* blob;
  /*
     There are two types of pointer, sensible and silly.
     Sensible pointers are created via valid pointer operations such
     as indexing into an array by an amount that doesn't exceed array
     bounds. However, in C you can do a lot of strange, strange operations.
     For instance, it is legal to increment a pointer well past the
     end of the array it references, then decrement it again and
     dereference it once it gets back into the array's range.

     So, the representation of offsets within a blob has two cases:
     When "path" is positive or zero, the pointer is sensible. The
     "type" field refers to the type of the top-level object in the
     blob and the "path" field refers to the exact part (as defined by
     the typemap) at which the pointer points.

     When the "path" field is negative, the pointer is silly. This is
     for pointers which cannot be described using the above system,
     for instance, (struct foo*)((char*)NULL + 47) is such a pointer.
     In these cases, the path field stores the offset in bytes
     from the start of the blob to where the pointer points.
  */
  usertype_t type;
  int32_t path;

  int silly;

  /* Sometimes, a programmer does weird shit and the type of the
     pointer bears no relation to the type stored in memory. In such
     cases, top_type and path still refer to what's actually in
     memory, while ctype gives the type the programmer thinks the
     pointer has. This is 0 in all sensible pointers. 
     
     We need to maintain this field in case a programmer casts a
     pointer to a silly type, offsets that pointer, and then
     dereferences it. This is the only way we have of knowing which
     value to return to the user.
  */
  usertype_t ctype;
};

/* returns the type of the object pointed to by this pointer.
   Argument must be a pointer type. */
usertype_t pointer_type(userptr_t);

/* returns a pointer to the start of a given blob */
userptr_t pointer_to_blob(struct blob*, usertype_t type);


/* index into an array */
userptr_t pointer_index(userptr_t, int index);

/* point to a member of a struct or union */
userptr_t pointer_offset(userptr_t, int field);

/* cast a pointer to a pointer of another type */
userptr_t pointer_cast(userptr_t, usertype_t newtype);

/* return what's pointed to by a given pointer. Pointer must point to
   a primitive */
primitive_val pointer_deref(userptr_t);

/* put something in the space pointed to by a given pointer. Must be a
   primitive, and pointer must point to a primitive */
void pointer_assign(userptr_t, primitive_val);


#endif
