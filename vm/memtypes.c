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
    usertype_t rtype = 
      (ptype & NBIT_MASK(24)) | (((ptype >> 24) & 0xff) + 1);
    assert(get_base_type_name(ptype) == get_base_type_name(rtype) &&
           get_ptr_levels(ptype) + 1 == get_ptr_levels(rtype));
    return rtype;
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
      byte_off += USER_SIZEOF_PTR;
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
  static const char* primitive_names[] = {
    [PT_VOID] = "void",
    [PS_CHAR] = "signed char",
    [PU_CHAR] = "unsigned char",
    [PS_SHORT] = "signed short",
    [PU_SHORT] = "unsigned short",
    [PS_INT] = "signed int",
    [PU_INT] = "unsigned int",
    [PS_LONG_LONG] = "signed long long",
    [PU_LONG_LONG] = "unsigned long long",
  };
  static const int primitive_sizes[] = {
    [PT_VOID] = 0,
    [PS_CHAR] = 1,
    [PU_CHAR] = 1,
    [PS_SHORT] = 2,
    [PU_SHORT] = 2,
    [PS_INT] = 4,
    [PU_INT] = 4,
    [PS_LONG_LONG] = 8,
    [PU_LONG_LONG] = 8,
  };

  assert(N_PRIMITIVE_DATA_TYPES == 
         sizeof(primitive_names)/sizeof(char*) &&
         N_PRIMITIVE_DATA_TYPES == 
         sizeof(primitive_sizes)/sizeof(int));

  for (int i=0;i<N_PRIMITIVE_DATA_TYPES;i++){
    typemap->names[i] = atom_get(primitive_names[i]);
    struct type_layout* l = malloc(sizeof(struct type_layout));
    memset(l, 0, sizeof(struct type_layout));
    typemap->layouts[i] = l;
    l->flavour = TF_PRIMITIVE;
    l->size = primitive_sizes[i];
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



#define DIV_ROUND_UP(a,b) ((a) + (b) - 1)/(b)

struct blob* blob_alloc(usersize_t size){
  struct blob* b = malloc(sizeof(struct blob));
  b->length = size;
  //Allocate a whole number of userdatas, enough to hold size bytes
  b->data = malloc(sizeof(userdata) * DIV_ROUND_UP(size, sizeof(userdata)));

  //FIXME: fill in b->data with something

  //The garbage map is allocated one bit per byte of userdata
  //This is big enough for anything (the most inefficient would be a char array)
  b->undef_map = bitset_alloc(size);
  return b;
}

usersize_t user_sizeof(usertype_t type){
  if (get_ptr_levels(type) > 0){
    return USER_SIZEOF_PTR;
  }else{
    return typemap->layouts[type]->size;
  }
}

atom get_base_type_name(usertype_t type){
  return typemap->names[type & NBIT_MASK(24)];
}




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
  */
  usertype_t top_type;
  int32_t path;
  usertype_t ctype;
};










static struct pointer* ptr_internals(userptr_t p){
  return ht_int_get(ptrtable, p);
}

static userptr_t ptr_new(struct blob* b, usertype_t top, uint32_t path, usertype_t ctype){
  struct pointer* newptr = malloc(sizeof(struct pointer));
  userptr_t id = max_pointer_id++;
  newptr->blob = b;
  newptr->top_type = top;
  newptr->path = path;
  newptr->ctype = ctype;
  ht_int_set(ptrtable, id, newptr);
  return id;
}




usertype_t pointer_type(userptr_t userptr){
  struct pointer* ptr = ptr_internals(userptr);
  if (ptr->ctype){
    //Programmer is doing something weird.
    return ptr->ctype;
  }else{

  }
}




userptr_t pointer_to_blob(struct blob* b, usertype_t type){
  return ptr_new(b, type, 0, 0);
}


userptr_t pointer_index(userptr_t base_ptr, int idx){
  struct pointer* base = ptr_internals(base_ptr);
  if (base->top_type){
    
  }else{
    
  }
}

userptr_t pointer_offset(userptr_t base_ptr, int field){
  struct pointer* base = ptr_internals(base_ptr);
  if (base->ctype){
    //oh shit...
  }else{
    if (base_ptr);
  }
}


userptr_t pointer_cast(userptr_t ptr, usertype_t newtype){
  struct pointer* p = ptr_internals(ptr);
  return ptr_new(p->blob, p->top_type, p->path, newtype == p->top_type ? 0 : newtype);
}
