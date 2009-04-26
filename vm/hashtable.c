#include "hashtable.h"
#include <assert.h>
#include "bitops.h"
#include <stdlib.h>

#define DEFAULT_SIZE 512

static char __deleted_elem = '\0';

//this is used to mark deleted elements
//it's a pointer that appears nowhere else in Altitude
#define DELETED (void*)(&__deleted_elem)


////// Atom -> void* hashtable


struct ht_atom{
  int capacity; // Must be power of two
  int filled;
  atom* keys;
  void** values;
};


ht_atom_t ht_atom_alloc(){
  ht_atom_t ht = malloc(sizeof(struct ht_atom));
  ht->capacity = DEFAULT_SIZE;
  ht->filled = 0;
  ht->keys = calloc(DEFAULT_SIZE, sizeof(atom));
  ht->values = calloc(DEFAULT_SIZE, sizeof(void*));
  return ht;
}
void ht_atom_free(ht_atom_t ht){
  free(ht->keys);
  free(ht->values);
  free(ht);
}
//Find a particular element, starting from a particular position
//Stop when null is found
static int ht_atom_keypos(ht_atom_t ht, atom key, int startpos, int del){
  //del indicates whether we should stop upon finding DELETED
  for (int i=startpos; i != ht->capacity; i++){
    if (ht->keys[i] == key || ht->keys[i] == NULL || (del && ht->keys[i] == DELETED)){
      return i;
    }
  }
  for (int i=0; i != startpos; i++){
    if (ht->keys[i] == key || ht->keys[i] == NULL || (del && ht->keys[i] == DELETED)){
      return i;
    }
  }
  //if the hashtable is correctly rehashed, this should never happen
  //FIXME: really?
  return startpos;
}
static int ht_hashpos(ht_atom_t ht, atom key){
  assert(IS_POWER_OF_TWO(ht->capacity));
  return key->hashcode & (ht->capacity - 1);
}
void* ht_atom_get(ht_atom_t ht, atom key){
  int pos = ht_atom_keypos(ht, key, ht_hashpos(ht, key), 0);
  if (ht->keys[pos]){
    return ht->values[pos];
  }else{
    return NULL;
  }
}


static int is_element(void* p){
  return (p != NULL) && (p != DELETED);
}

void ht_atom_set(ht_atom_t ht, atom key, void* val){
  //find deleted or null element
  int pos = ht_atom_keypos(ht, key, ht_hashpos(ht, key), 1);
  if (is_element(ht->keys[pos]) && val == NULL){
    ht->filled--;
  }
  if (!is_element(ht->keys[pos]) && val != NULL){
    ht->filled++;
  }
  if (!val){
    ht->keys[pos] = DELETED;
  }else{
    ht->keys[pos] = key;
    ht->values[pos] = val;
  }
  
  //We may need to rehash at this point
  int newsize = ht->capacity;
  int fullness = (100 * ht->filled) / ht->capacity;
  if (fullness > 60) newsize = ht->capacity * 2;
  else if (fullness < 10) newsize = ht->capacity / 2;
  if (newsize < DEFAULT_SIZE) newsize = ht->capacity;

  if (newsize != ht->capacity){
    //rehash!
    atom* oldkeys = ht->keys;
    void** oldvals = ht->values;
    int oldcap = ht->capacity;
    ht->keys = calloc(newsize, sizeof(atom));
    ht->values = calloc(newsize, sizeof(void*));
    ht->capacity = newsize;
    ht->filled = 0;
    for (int i=0;i<oldcap;i++){
      if (is_element(oldkeys[i]))
	ht_atom_set(ht, oldkeys[i], oldvals[i]);
    }
  }
}


void ht_atom_del(ht_atom_t ht, atom key){
  ht_atom_set(ht, key, NULL);
}



//Iteration

ht_atom_iter ht_atom_begin(ht_atom_t ht){
  ht_atom_iter i = {ht, -1};
  return ht_atom_next(i);
}
atom ht_atom_getkey(ht_atom_iter i){
  return i.ht->keys[i.idx];
}
void* ht_atom_getvalue(ht_atom_iter i){
  return i.ht->values[i.idx];
}
ht_atom_iter ht_atom_next(ht_atom_iter i){
  do{
    i.idx ++;
  } while (i.idx < i.ht->capacity && !is_element(i.ht->keys[i.idx]));
  return i;
}
int ht_atom_hasmore(ht_atom_iter i){
  return i.idx < i.ht->capacity;
}




//// uint64_t -> void* hashtable. Cut-and-paste of the above code


struct ht_int{
  int capacity; // Must be power of two
  int filled;
  uint64_t* keys;
  void** values;
};



ht_int_t ht_int_alloc(){
  ht_int_t ht = malloc(sizeof(struct ht_int));
  ht->capacity = DEFAULT_SIZE;
  ht->filled = 0;
  ht->keys = calloc(DEFAULT_SIZE, sizeof(uint64_t));
  ht->values = calloc(DEFAULT_SIZE, sizeof(void*));
  return ht;
}
void ht_int_free(ht_int_t ht){
  free(ht->keys);
  free(ht->values);
  free(ht);
}
//Find a particular element, starting from a particular position
//Stop when null is found
static int ht_int_keypos(ht_int_t ht, uint64_t key, int startpos, int del){
  //del indicates whether we should stop upon finding DELETED
  for (int i=startpos; i != ht->capacity; i++){
    if (ht->keys[i] == key || ht->values[i] == NULL || (del && ht->values[i] == DELETED)){
      return i;
    }
  }
  for (int i=0; i != startpos; i++){
    if (ht->keys[i] == key || ht->values[i] == NULL || (del && ht->values[i] == DELETED)){
      return i;
    }
  }
  //if the hashtable is correctly rehashed, this should never happen
  //FIXME: really?
  return startpos;
}
static int ht_int_hashpos(ht_int_t ht, uint64_t key){
  assert(IS_POWER_OF_TWO(ht->capacity));
  return (int)key & (ht->capacity - 1);
}
void* ht_int_get(ht_int_t ht, uint64_t key){
  int pos = ht_int_keypos(ht, key, ht_int_hashpos(ht, key), 0);
  if (ht->values[pos] != DELETED){
    return ht->values[pos];
  }else{
    return NULL;
  }
}



void ht_int_set(ht_int_t ht, uint64_t key, void* val){
  //find deleted or null element
  int pos = ht_int_keypos(ht, key, ht_int_hashpos(ht, key), 1);
  if (is_element(ht->values[pos]) && val == NULL){
    ht->filled--;
  }
  if (!is_element(ht->values[pos]) && val != NULL){
    ht->filled++;
  }
  if (!val){
    ht->values[pos] = DELETED;
  }else{
    ht->keys[pos] = key;
    ht->values[pos] = val;
  }
  
  //We may need to rehash at this point
  int newsize = ht->capacity;
  int fullness = (100 * ht->filled) / ht->capacity;
  if (fullness > 60) newsize = ht->capacity * 2;
  else if (fullness < 10) newsize = ht->capacity / 2;
  if (newsize < DEFAULT_SIZE) newsize = ht->capacity;

  if (newsize != ht->capacity){
    //rehash!
    uint64_t* oldkeys = ht->keys;
    void** oldvals = ht->values;
    int oldcap = ht->capacity;
    ht->keys = calloc(newsize, sizeof(uint64_t));
    ht->values = calloc(newsize, sizeof(void*));
    ht->capacity = newsize;
    ht->filled = 0;
    for (int i=0;i<oldcap;i++){
      if (is_element(oldvals[i]))
	ht_int_set(ht, oldkeys[i], oldvals[i]);
    }
  }
}


void ht_int_del(ht_int_t ht, uint64_t key){
  ht_int_set(ht, key, NULL);
}



//Iteration

ht_int_iter ht_int_begin(ht_int_t ht){
  ht_int_iter i = {ht, -1};
  return ht_int_next(i);
}
uint64_t ht_int_getkey(ht_int_iter i){
  return i.ht->keys[i.idx];
}
void* ht_int_getvalue(ht_int_iter i){
  return i.ht->values[i.idx];
}
ht_int_iter ht_int_next(ht_int_iter i){
  do{
    i.idx ++;
  } while (i.idx < i.ht->capacity && !is_element(i.ht->values[i.idx]));
  return i;
}
int ht_int_hasmore(ht_int_iter i){
  return i.idx < i.ht->capacity;
}
