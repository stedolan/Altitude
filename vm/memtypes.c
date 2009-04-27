#include <stdlib.h>

#include "memtypes.h"
#include "bitops.h"
#include "hashtable.h"
#include "error.h"
#include "sexp.h"


static struct typemap* typemap;



struct type_layout* get_type_layout(usertype_t type){
  int typeidx = type & NBIT_MASK(24);
  if (typeidx < 0 || typeidx >= typemap->ntypes){
    sayf(ERROR, "trying to get layout of invalid type (type %d, but only %d exist)", 
	 typeidx, typemap->ntypes);
  }
  return typemap->layouts[typeidx];
}



struct blob* blob_alloc(usersize_t size){
  struct blob* b = malloc(sizeof(struct blob));
  b->length = size;
  //Allocate a whole number of userdatas, enough to hold size bytes
  b->data = malloc(sizeof(userdata) * DIV_ROUND_UP(size, sizeof(userdata)));

  //FIXME: fill in b->data with something

  //The garbage map is allocated one bit per byte of userdata
  //This is big enough for anything (the most inefficient would be a char array)
  b->undef_map = bitset_alloc(size);
}

usersize_t user_sizeof(usertype_t type){
  if (get_ptr_levels(type) > 0){
    return USER_SIZEOF_PTR;
  }else{
    return get_base_type(type)->size;
  }
}

atom get_base_type_name(usertype_t type){
  return typemap->names[type & NBIT_MASK(24)];
}




struct pointer* ptr_internals(userptr_t){
  return ht_int_get(something...);
}
static userptr_t ptr_new(struct blob* b, usertype_t top, uint32_t path, usertype_t ctype){
  struct pointer* newptr = malloc(sizeof(struct pointer));
  userptr_t id = someid++;
  newptr->blob = b;
  newptr->top_type = top;
  newptr->path = path;
  newptr->ctype = ctype;
  ht_int_set(something...);
  return id;
}

usertype_t pointer_type(userptr_t userptr){
  struct pointer* ptr = ptr_internals(userptr);
  if (ptr->ctype){
    //Programmer is doing something weird.
    return ptr->ctype;
  }else{
    //Ok, normal pointer. top_type and path are meaningful.
    struct type_layout* blobtype = get_type_layout(ptr->top_type);
    if (ptr->path < 0 || ptr->path >= blobtype->nchildren){
      say(ERROR, "invalid path in pointer");
    }
    return blobtype->children[ptr->path];
  }
}


userptr_t pointer_to_blob(struct blob* b, usertype_t type){
  return ptr_new(b, type, 0, 0);
}


userptr_t pointer_offset(userptr_t base_ptr, int field){
  struct pointer* base = ptr_internals(base_ptr);
  if (base->ctype){
    //oh shit...
  }else{
    if (base_ptr
  }
}

