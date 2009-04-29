#include <stdlib.h>
#include <string.h>

#include "memtypes.h"
#include "bitops.h"
#include "hashtable.h"
#include "error.h"
#include "sexp.h"
#include <assert.h>

/* First half or so of this file is types and typemap,
   second is pointer manipulation */

typedef enum{TF_STRUCT, TF_UNION, TF_ARRAY, TF_PRIMITIVE} type_flavour;

/* the layout of a single type */
struct type_layout{
  type_flavour flavour;
  usersize_t size;

  int nchildren;
  usertype_t* children; //type of the object at each path, indexed by 0..nchildren
  usersize_t* bytepos; //byte offsets from start of object

  int nfields; //For structure/union types, number of fields
  atom* fieldnames;
  int* fieldpaths;//indices into children and bytepos for each field
  
  struct sexp* definition;
};

/* the typemap for all types in the program */
struct typemap{
  int type_capacity;
  int ntypes;
  /* The names are kept seperately to the layouts as multiple types
     can share a layout (i.e. typedefs) */
  atom* names; //names of the types
  struct type_layout** layouts; //an array which can be indexed by usertype_t
};

static inline usertype_t make_pointer_type(usertype_t base, int ptrlevels){
  usertype_t rtype = 
    (base & NBIT_MASK(24)) | (((base >> 24) & 0xff) + ptrlevels);
  assert(get_base_type_name(base) == get_base_type_name(rtype) &&
         get_ptr_levels(base) + ptrlevels == get_ptr_levels(rtype));
  return rtype;
}


// ceil(a/b), for positive b and non-negative a
#define DIV_ROUND_UP(a,b) ((a) + (b) - 1)/(b)

// min x such that x >= a and x % b == 0
// for positive b and non-negative a
#define ROUND_UP(a,b) DIV_ROUND_UP(a,b)*b


static struct typemap* typemap;
static ht_int_t ptrtable;
static userptr_t max_pointer_id = 0;

static int typeidx_by_name(atom name){
  for (int i=0;i<typemap->ntypes;i++){
    if (typemap->names[i] == name)return i;
  }

  //not found, create it instead
  if (typemap->ntypes >= typemap->type_capacity){
    typemap->type_capacity *= 3;
    typemap->names = realloc(typemap->names, sizeof(atom) * typemap->type_capacity);
    typemap->layouts = realloc(typemap->layouts, sizeof(struct typemap*) * typemap->type_capacity);
  }
  typemap->names[typemap->ntypes] = name;
  typemap->layouts[typemap->ntypes] = malloc(sizeof(struct type_layout));
  memset(typemap->layouts[typemap->ntypes], 0, sizeof(struct type_layout));
  return typemap->ntypes++;
}


//FIXME: usage of assert is odd
static usertype_t type_component_from_sexp(struct sexp* t){
  switch (t->tag){
  case S_INT:return (usertype_t)PS_INT;
  case S_PTR:{
    assert(t->nelems==1 && t->elems[0].type==ST_SEXP);
    usertype_t ptype = 
      type_component_from_sexp(t->elems[0].data.sexp);
    return make_pointer_type(ptype, 1);
  }
  case S_COMPTYPE:
    assert(t->nelems==1 && t->elems[0].type==ST_STRING);
    return (usertype_t)(typeidx_by_name(t->elems[0].data.string));
  case S_ARRAY:
    //FIXME
    return 0;
  case S_TYPEREF:
    //FIXME:
    return 0;
  default:
    //FIXME
    assert(0);
    return 0;
  }
}

usertype_t type_from_sexp(struct sexp* s){
  assert(s->tag == S_TYPE &&
         s->nelems == 1 &&
         s->elems[0].type == ST_SEXP);
  return type_component_from_sexp(s->elems[0].data.sexp);
}



static void build_type(int idx){
  //convert the sexp-definition to a typemap entry
  struct type_layout* l = typemap->layouts[idx];
  if (! l->definition) return; //already built
  struct sexp* fieldlist = l->definition;

  l->nfields = fieldlist->nelems;
  l->fieldnames = malloc(sizeof(atom) * fieldlist->nelems);
  l->fieldpaths = malloc(sizeof(int) * fieldlist->nelems);


  //the first "child" is ourself
  int nchildren = 1;   //recursive incest
  for (int i=0;i<fieldlist->nelems;i++){
    struct sexp* field = fieldlist->elems[i].data.sexp;
    if (!(field->nelems == 2 &&
          is_sexp_with_tag(field->elems[0], S_TYPE) &&
	  field->elems[1].type == ST_STRING))goto bad;
    usertype_t fieldtype = type_from_sexp(field->elems[0].data.sexp);
    atom fieldname = field->elems[1].data.string;
    l->fieldnames[i] = fieldname;
    l->fieldpaths[i] = nchildren;
    if (!is_ptr_type(fieldtype)){
      build_type(fieldtype);
      nchildren += typemap->layouts[fieldtype]->nchildren;
    }else{
      nchildren++;
    }
    
  }



  l->children = malloc(sizeof(usertype_t)*nchildren);
  l->bytepos = malloc(sizeof(uint32_t)*nchildren);
  l->nchildren = nchildren;
  usersize_t byte_off = 0;
  int path_off = 0;
  l->children[path_off++] = idx;//first child is self
  for (int i=0;i<fieldlist->nelems;i++){
    struct sexp* field = fieldlist->elems[i].data.sexp;
    usertype_t fieldtype = type_from_sexp(field->elems[0].data.sexp);
    if (is_ptr_type(fieldtype)){
      l->children[path_off] = fieldtype;
      l->bytepos[path_off] = byte_off;
      path_off ++;
      if (l->flavour == TF_STRUCT || l->flavour == TF_UNION)
        byte_off += ROUND_UP(PRIM_USERSIZE(PT_PTR), USERSIZE_USERDATA);
      else
        byte_off += PRIM_USERSIZE(PT_PTR);
    }else{
      struct type_layout* child_l = typemap->layouts[fieldtype];
      memcpy(l->children + path_off, child_l->children, 
             child_l->nchildren * sizeof(usertype_t));
      memcpy(l->bytepos + path_off, child_l->bytepos,
             child_l->nchildren * sizeof(uint32_t));
      for (int i=0;i<child_l->nchildren;i++){
        l->bytepos[path_off + i] += byte_off;
      }
      path_off += child_l->nchildren;
      if (l->flavour == TF_STRUCT || l->flavour == TF_UNION)
        byte_off += ROUND_UP(child_l->size, USERSIZE_USERDATA);
      else
        byte_off += child_l->size;
    }
  }
  l->size = byte_off;
  assert(path_off == nchildren);

  //we don't need the original sexp anymore, get rid of it to mark this
  //as done
  l->definition = NULL;
  return;

 bad:
  //oh shit
  //FIXME: proper error propagation
  say(ERROR, "bad things here");
}





void build_typemap(int ntypes, struct sexp** types){
  ptrtable = ht_int_alloc();
  typemap = malloc(sizeof(struct typemap));
  typemap->ntypes = 0;
  typemap->type_capacity = 100;
  typemap->names = malloc(sizeof(atom) * typemap->type_capacity);
  typemap->layouts = malloc(sizeof(struct type_layout*) * typemap->type_capacity);

  //First of all, add the primitive types
  //Type 0 is void, a pseudo-type used to denote
  //uninitialised memory

  for (int i=0;i<N_PRIMITIVE_DATA_TYPES;i++){
    typemap->names[i] = atom_get(PRIM_C_NAME(i));
    struct type_layout* l = malloc(sizeof(struct type_layout));
    memset(l, 0, sizeof(struct type_layout));
    typemap->layouts[i] = l;
    l->flavour = TF_PRIMITIVE;
    l->size = PRIM_USERSIZE(i);
    l->nchildren = 1;
    l->children = malloc(sizeof(usertype_t));
    l->children[0] = (usertype_t)i;
    l->bytepos = malloc(sizeof(usersize_t));
    l->bytepos[0] = 0;
  }

        
  for (int i=0;i<ntypes;i++){
    struct sexp* t = types[i];
    if (t->tag == S_STRUCT || t->tag == S_UNION){
      if (!(t->nelems == 2 && 
	    t->elems[0].type == ST_STRING &&
	    is_sexp_with_tag(t->elems[1], S_FIELDS))){
	goto bad;
      }
      atom name = t->elems[0].data.string;
      int typeidx = typeidx_by_name(name);
      typemap->layouts[typeidx]->definition = t->elems[1].data.sexp;
      if (t->tag == S_STRUCT)
        typemap->layouts[typeidx]->flavour = TF_STRUCT;
      else if (t->tag == S_UNION)
        typemap->layouts[typeidx]->flavour = TF_UNION;
    }else{
      goto bad;
    }
  }
  for (int i=0;i<typemap->ntypes;i++){
    build_type(i);
  }

  return;

 bad:
  say(TYPEMAP, "Bad structure sexp");
}



int get_type_offset(atom base, atom field){
  struct type_layout* l = 
    typemap->layouts[typeidx_by_name(base)];
  for (int i=0;i<l->nfields;i++){
    if (l->fieldnames[i] == field)
      return l->fieldpaths[i];
  }
  //FIXME: err
  sayf(TYPEMAP, "couldn't find field {%s} in {%s}", 
       field->string, base->string);
  assert(0);
}



usersize_t user_sizeof(usertype_t type){
  if (get_ptr_levels(type) > 0){
    return PRIM_USERSIZE(PT_PTR);
  }else{
    return typemap->layouts[type]->size;
  }
}

atom get_base_type_name(usertype_t type){
  return typemap->names[type & NBIT_MASK(24)];
}


// The pointer manipulation (dereferencing, indexing, etc) code.

// Here be dragons.



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

     So, the representation of offsets within a blob has multiple
     cases:
     
     When top_type is non-null, the top_type and path point to a valid
     location (assuming the blob is big enough, that's checked
     elsewhere). If ctype is null, the type of the pointer (as it
     would be declared in C) is the type implied by the top_type and
     path.
     
     If ctype is not null, it means that the pointer was cast to
     something other that the type of the object it seems to point to,
     for instance, a pointer pointing to an int was temporarily cast
     to a struct foo*. When it is a struct foo*, the ctype will be
     struct foo* but the top_type and path will still say int. When it
     is cast back, it can be verified that the types are valid.

     If top_type is null and ctype is non-null, it means that the
     programmer is doing truly weird shit, and Altitude has abandoned
     him to his sorry fate. For instance, this occurs when you cast a
     pointer to a int to a struct foo*, increment it by 17 *
     sizeof(long long), and add the offset of some field of struct
     foo.

     In this case, we can't really figure out what was intended, but
     we still have to give the right results if the programmer does
     all of those steps in reverse (that is, even though the pointer
     is invalid, we still have to store exactly how invalid it
     is). Since we have nothing sensible to put in path, we use it to
     store a byte offset from the start of the struct. This is why it
     is a signed integer type, as a truly silly pointer can refer to
     the -1000'th element of an array. (This is totally valid, but you
     have to increment it by 1000 before you can dereference
     it. Sadly, people do this kind of thing, and C expressly condones
     it).

     top_type and ctype are never both null at once. If both are present,
     they are never equal (the situation of top_type == ctype has the same
     effect as ctype == NULL, the latter is preferred).

     There is one other special case (yay!). If "blob" is NULL, then
     this pointer is a function pointer, given by path.
  */
  usertype_t top_type;
  int32_t path;
  usertype_t ctype;
};










static struct pointer* ptr_internals(userptr_t p){
  return ht_int_get(ptrtable, p);
}

static inline userptr_t ptr_new(struct blob* b, usertype_t top, int32_t path, usertype_t ctype){
  struct pointer* newptr = malloc(sizeof(struct pointer));
  userptr_t id = max_pointer_id++;
  newptr->blob = b;
  newptr->top_type = top;
  newptr->path = path;
  newptr->ctype = ctype;
  ht_int_set(ptrtable, id, newptr);
  return id;
}


userptr_t pointer_to_function(int fidx){
  return ptr_new(NULL, 0, fidx, 0);
}


/* Must not be called with function pointers */
usertype_t pointer_type(userptr_t userptr){
  struct pointer* ptr = ptr_internals(userptr);
  if (ptr->ctype){
    //Programmer is doing something weird.
    return ptr->ctype;
  }else{
    if (is_ptr_type(ptr->top_type)){
      return make_pointer_type(ptr->top_type, -1);
    }else{
      struct type_layout* layout = typemap->layouts[ptr->top_type];
      return layout->children[ptr->path];
    }
  }
}


//Given that an object appears at point "path" in a blob
//with toptype "top", what index is it at in the array
//stored in that blob? This should be a simple division of
//path by the number of possible paths for that type,
//but that would fail for negative paths (C rounding behaviour
//is implementation-defined for negative integer division)
static inline int get_arrpos(usertype_t top, int path){
  int pos = abs(path) / typemap->layouts[top]->nchildren;
  return (path >= 0) ? pos : -pos - 1;
}
//same, for ctypes
static inline int get_arrpos_ctype(usertype_t ctype, int path){
  int pos = abs(path) / typemap->layouts[ctype]->size;
  return (path >= 0) ? pos : -pos - 1;
}

//Return offset from the start of the blob, in bytes
//Must not be called with function pointers
static int byte_offset(struct pointer* p){
  if (p->top_type){
    //convert from path to byte count
    usertype_t type = p->top_type;
    int path = p->path;
    int arrpos = get_arrpos(p->top_type, path);
    int objoff;
    int elem_len;
    if (is_ptr_type(type)){
      objoff = 0;
      elem_len = PRIM_USERSIZE(PT_PTR);
    }else{
      struct type_layout* layout = typemap->layouts[type];
      elem_len = layout->nchildren;
      int rem_path = path - arrpos * elem_len;
      assert(rem_path >= 0 && rem_path < layout->nchildren);
      objoff = layout->bytepos[rem_path];
    }
    return arrpos * elem_len + objoff;
  }else{
    //path is already in bytes
    return p->path;
  }
}





struct blob* blob_alloc(usersize_t size, usertype_t type){
  struct blob* b = malloc(sizeof(struct blob));
  b->length = size;
  //Allocate a whole number of userdatas, enough to hold size bytes
  b->data = malloc(sizeof(userdata) * DIV_ROUND_UP(size, sizeof(userdata)));

  //FIXME: fill in b->data with something better
  memset(b->data, 42, size);
  
  b->pointer = ptr_new(b, type, 0, 0);
  b->decltype = type;
  
  //The valid map is allocated one bit per byte of userdata
  //This is big enough for anything (the most inefficient would be a char array)
  b->valid_map = bitset_alloc(size);
  bitset_clear(b->valid_map);
  return b;
}

userptr_t pointer_to_blob(struct blob* b, usertype_t type){
  if (type == b->decltype)return b->pointer;
  else return ptr_new(b,type,0,0);
}


/* This function should satisfy the following property:
   pointer_index(pointer_index(ptr, i1), i2)
                      ==
   pointer_index(ptr, i1 + i2)
   (where equality is defined as equivalence, not C integer '==') */
int pointer_index(userptr_t* ret, userptr_t base_ptr, int idx){
  struct pointer* ptr = ptr_internals(base_ptr);
  if (!ptr->blob){
    //function pointer. Bad programmer. Stop that.
    //Someday, we'll implement arrays of function pointers
    //Today is not that day.

    say(PTR_CHANGE_INVAL, "Trying to index function pointer");
    return 0;
  }
  usertype_t top_type = ptr->top_type, ctype = ptr->ctype;
  int path = ptr->path;
  if (top_type){
    //we can only cleanly index a top_type pointer if the index
    //is on path 0 (+/- a multiple of nchildren). This corresponds
    //to indexing an array of structs by iterating over whole structs
    //rather than silly things like iterating over individual fields as
    //though they were arrays.
    if (!is_ptr_type(top_type) &&
        abs(path) % typemap->layouts[top_type]->nchildren != 0){
      //the programmer is trying to use an internal member
      //of an object (e.g. a struct field) as an array, and index into it.
      //
      //the programmer is a bad person

      //Eventually, when we implement member arrays, this will be valid

      //convert to a ctype pointer and carry on
      ctype = pointer_type(base_ptr);
      path = byte_offset(ptr);
      top_type = 0;
    }
  }
  if (ctype){
    //An offset is being applied while the pointer has a ctype
    //This will almost certainly result in a garbage pointer, but
    //we have to do it anyway. In theory, the user could undo their 
    //changes exactly and still end up with a dereferenceable pointer
    if (is_ptr_type(ctype))
      path += idx * PRIM_USERSIZE(PT_PTR);
    else
      path += idx * typemap->layouts[ctype]->size;
  }else{
    path += idx * typemap->layouts[top_type]->nchildren;
  }
  *ret = ptr_new(ptr->blob, top_type, path, ctype);
  return 1;
}

/* This function should satisfy the following property:
   pointer_offset(pointer_offset(ptr, o1), o2)
                      ==
   pointer_offset(ptr, o1 + o2)
   (where equality is defined as equivalence, not C integer '==') */
int pointer_offset(userptr_t* ret, userptr_t base_ptr, int field){
  struct pointer* ptr = ptr_internals(base_ptr);
  if (!ptr->blob){
    //you can't do this to a function pointer
    //FIXME: error message;
    say(PTR_CHANGE_INVAL, "Trying to get a field of a function");
    return 0;
  }

  if (ptr->ctype){
    //An offset is being applied while the pointer has a ctype
    //This will almost certainly result in a garbage pointer, but
    //we have to do it anyway. In theory, the user could undo their 
    //changes exactly and still end up with a dereferenceable pointer
    assert(!is_ptr_type(ptr->ctype));
    struct type_layout* layout = typemap->layouts[ptr->ctype];
    assert(field >= 0 && field < layout->nfields);
    say(PTR_CHANGE, "Suspicious pointer arithmetic");
    *ret = ptr_new(ptr->blob,
                   0,
                   ptr->path + layout->bytepos[layout->fieldpaths[field]],
                   layout->children[layout->fieldpaths[field]]);
  }else{
    assert(!is_ptr_type(ptr->top_type));
    assert(ptr->path + field < typemap->layouts[ptr->top_type]->nchildren);
    *ret = ptr_new(ptr->blob,
                   ptr->top_type,
                   ptr->path + field,
                   0);
  }
  return 1;
}


//Make a pointer point to a new top_type without changing
//what it points to or the type it points to
//Used to figure out whether a read/write through an unusual
//type of pointer is valid (oh god, C! why you do?).
static inline int try_safe_cast(struct pointer* p, usertype_t new_top_type){
  //If it's a ctype, we have no hope
  if (!p->top_type)return 0;

  //are we already done?
  if (p->top_type == new_top_type)return 1;

  //if the types being cast from and to are both pointer types,
  //well and good (you can store any ptr in any other ptr in C)
  if (is_ptr_type(p->top_type) && is_ptr_type(new_top_type))return 1;
  //if either (but not both) are pointer types, bad.
  if (is_ptr_type(p->top_type) || is_ptr_type(new_top_type))return 0;

  int arrpos = get_arrpos(p->top_type, p->path);

  //if this isn't the first element of the array, bad things
  //FIXME: there is one case where this is semi-valid, that's
  //when old and new types are primitive integral types 
  //differing only in signedness. Implement later, if at all.
  if (arrpos != 0)return 1;

  //Now, we need to find out if the new type has the right element
  //in the right plase
  usertype_t desired_type = p->ctype ? 
    p->ctype : 
    typemap->layouts[p->top_type]->children[p->path];
  
  //now try to "unify" (in the Prolog sense) the type of the pointer
  //and the desired type.
  
  //FIXME: for now, don't bother
  return 0;
}

int pointer_cast(userptr_t* ret, userptr_t ptr, usertype_t newtype){
  struct pointer* p = ptr_internals(ptr);
  if (!p->blob){
    //FIXME: this, technically, is valid. Damn!
    say(PTR_CAST, "Casting function pointers is not supported (yet)");
    return 0;
  }
  if (p->top_type && p->path == 0){
    //FIXME: strictly, we should do this for signed-unsigned casts as well
    *ret = ptr_new(p->blob, newtype, 0, 0);
    return 1;
  }else{
    *ret = ptr_new(p->blob, p->top_type, p->path, newtype == p->top_type ? 0 : newtype);
    return 1;
  }
}

int pointer_deref_function(int* ret, userptr_t ptr){
  struct pointer* p = ptr_internals(ptr);
  if (p->blob){
    //FIXME: error
    say(INVALID_READ, "Trying to call something which isn't a function");
    return 0;
  }
  *ret = p->path;
  return 1;
}

static int blob_read(struct blob* b, int pos, primitive_val* val){
  //FIXME: alignment restrictions
  //FIXME: better errors
  if (pos & 7){
    say(UNALIGNED_READ, "Bad read");
    val->valid = 0;
    return 0;
  }
  if (pos < 0 || pos >= b->length){
    say(INVALID_READ, "Out-of-bounds memory access");
    val->valid = 0;
    return 0;
  }
  int idx = pos >> 3;
  val->value = b->data[idx];
  val->valid = bitset_get(b->valid_map, pos);
  if (!val->valid){
    say(DUBIOUS_READ, "Seemingly uninitialised or undefined value read");
  }
  return 1;
}

int pointer_deref(primitive_val* ret, userptr_t ptr){
  struct pointer* p = ptr_internals(ptr);
  if (!p->blob){
    //dereferencing a function pointer gives itself.
    //no, I don't know why.
    ret->type = PT_PTR;
    ret->valid = 1;
    USERDATA_PART(ret->value,PT_PTR) = ptr;
    return 1;
  }else{
    usertype_t t = pointer_type(ptr);
    if (is_ptr_type(t)){
      ret->type = PT_PTR;
    }else{
      assert(t && t < N_PRIMITIVE_DATA_TYPES);
      ret->type = (primtype)t;
    }
    if (p->ctype){
      say(DUBIOUS_READ, "Reading through a pointer not known to be valid");
      return blob_read(p->blob, p->path, ret);
    }else{
      struct pointer fixed = *p;
      if (try_safe_cast(&fixed, p->top_type)){
        //oh good, this is valid
        return blob_read(p->blob, byte_offset(&fixed), ret);
      }else{
        say(DUBIOUS_READ, "Seemingly badly-typed read");
        //reading garbage, most likely. Do it anyway.
        int status = blob_read(p->blob, byte_offset(p), ret);
        //even though there may have been something
        //valid at that address, this read was not valid.
        ret->valid = 0;
        return status;
      }
    }
  }
}

static int blob_write(struct blob* b, int pos, primitive_val val){
  //FIXME: alignment restrictions aren't this tight,
  //especially for arrays of a primitive type
  if (pos & 7){
    say(UNALIGNED_WRITE, "Bad write");
    return 0;
  }
  if (pos <0 || pos >= b->length){
    say(INVALID_WRITE, "Out-of-bounds write");
    return 0;
  }
  int idx = pos >> 3;

  //FIXME: what happens when small types are written
  //FIXME: endian-ness
  b->data[idx] = val.value;
  bitset_set(b->valid_map, pos, 1);
  return 1;
}

int pointer_assign(userptr_t ptr, primitive_val val){
  struct pointer* p = ptr_internals(ptr);
  if (!p->blob){
    //trying to assign to a function
    //FIXME: error message
    say(INVALID_READ, "Trying to assign to a function");
    return 0;
  }else{
    assert(is_ptr_type(pointer_type(ptr)) || pointer_type(ptr) == val.type);
    if (p->ctype){
      say(DUBIOUS_WRITE, "Writing through a pointer not known to be valid");
      return blob_write(p->blob, p->path, val);
    }else{
      struct pointer fixed = *p;
      if (try_safe_cast(&fixed, p->top_type)){
        //the pointer "fits" into what's actually in memory. Oh good.
      }else{
        //the pointer doesn't fit, invalidate the entire blob
        say(DUBIOUS_WRITE, "Write has invalidated previous contents of memory");
        p->blob->top_type = p->top_type;
        bitset_clear(p->blob->valid_map);
      }
      assert(byte_offset(&fixed) == byte_offset(p));
      return blob_write(p->blob, byte_offset(&fixed), val);
    }
  }
}
