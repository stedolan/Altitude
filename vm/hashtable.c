#include "hashtable.h"
#include <assert.h>
#include "bitops.h"
#include <stdlib.h>

#define DEFAULT_SIZE 512

struct ht_atom{
  int capacity; // Must be power of two
  int filled;
  atom* keys;
  void** values;
};

static char __deleted_elem = '\0';

//this is used to mark deleted elements
//it's a pointer that appears nowhere else in Altitude
#define DELETED (void*)(&__deleted_elem)

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
static int ht_keypos(ht_atom_t ht, atom key, int startpos, int del){
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
  int pos = ht_keypos(ht, key, ht_hashpos(ht, key), 0);
  if (ht->keys[pos]){
    return ht->values[pos];
  }else{
    return NULL;
  }
}


int is_element(void* p){
  return (p != NULL) && (p != DELETED);
}

void ht_atom_set(ht_atom_t ht, atom key, void* val){
  //find deleted or null element
  int pos = ht_keypos(ht, key, ht_hashpos(ht, key), 1);
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


//Iteration

void ht_atom_del(ht_atom_t ht, atom key){
  ht_atom_set(ht, key, NULL);
}

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
  } while (i.idx < i.ht->capacity && 
	   (i.ht->keys[i.idx] == NULL ||
	    i.ht->keys[i.idx] == DELETED));
  return i;
}
int ht_atom_hasmore(ht_atom_iter i){
  return i.idx < i.ht->capacity;
}
