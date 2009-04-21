#ifndef _ALTITUDE_MEMTYPES_H
#define _ALTITUDE_MEMTYPES_H


/* This is quite a complex file.
 * This contains most of the definitions for the Altitude VM's
 * internal structures, and the memory model.
 *
 */


#include <stdint.h>
#include "bitops.h"


/* Userdata
 * All user-space data is stored in userdata objects, which are 64-bit
 * unions of either 8 chars, 4 shorts, 2 ints, a 64-bit int or a
 * pointer.
 */

typedef enum primtype{
  S_CHAR, /* plain "char" in Altitude is signed char */
  U_CHAR,
  S_SHORT,
  U_SHORT,
  //int and long are equivalent (both 32 bits)
  S_INT,
  U_INT,
  //long long is 64 bits
  S_LONG_LONG,
  U_LONG_LONG,
  T_PTR,
};


static const int USERDATA_CHAR_LEN = sizeof(uint64_t);
static const int USERDATA_SHORT_LEN = sizeof(uint64_t) / sizeof(uint16_t);
static const int USERDATA_INT_LEN = sizeof(uint64_t) / sizeof(uint32_t);
static const int USERDATA_LL_LEN = 1;
static const int USERDATA_PTR_LEN = 1;


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



typedef enum{TF_STRUCT, TF_UNION, TF_TYPEDEF, TF_PRIMITIVE} type_flavour;
/* the layout of a single type */
struct type_layout{
  type_flavour flavour;
  int nchildren;
  usertype_t* children;
};

/* the typemap for all types in the program */
struct typemap{
  int ntypes;
  struct type_layout* layouts; //an array which can be indexed by usertype_t
};

/* size of a user type */
usersize_t user_sizeof(usertype_t);






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




/* A pointer in userspace. Not represented as a pointer in Altitude,
   but as a 64-bit key in Altitude's internal pointer-table */
typedef uint64_t userptr_t;


struct pointer{
  userptr_t id;
  struct blob* blob;

  usertype_t top_type;
  uint32_t path;

  usertype_t ctype;
};

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
