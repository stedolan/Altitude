
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "atom.h"

#define DEFAULT_SIZE 1024
// hashtable mapping strings to atoms
// Items are stored in linked lists using the "next" field of atoms
struct{
  int capacity; // Must be a power of two
  int emptybuckets;
  int totalitems;
  atom* values;//array of size capacity. Each is the head of a linked-list.
} atomtable;
static uint64_t max_id;

/* make sure hashtable is initialised */
static void table_init(){
  if (!atomtable.values){
    atomtable.capacity = DEFAULT_SIZE;
    atomtable.emptybuckets = atomtable.capacity;
    atomtable.totalitems = 0;
    atomtable.values = calloc(atomtable.capacity, sizeof(atom));
  }
}

/* rehash if necessary */
static void table_rehash(){
  //these are percentages
  int buckets_used = 100 - (100 * atomtable.emptybuckets) / atomtable.capacity;
  int fullness = (100 * atomtable.totalitems) / atomtable.capacity;

  int newsize = atomtable.capacity;
  if (buckets_used > 70 || fullness > 90){
    newsize = atomtable.capacity * 2;
  }else if (buckets_used < 15 && fullness < 25){
    newsize = atomtable.capacity / 2;
  }
  if (newsize < DEFAULT_SIZE) newsize = DEFAULT_SIZE;
  if (newsize != atomtable.capacity){
    //Rehash, making bigger or smaller
    int oldcap = atomtable.capacity;
    atom* oldvals = atomtable.values;
    atomtable.capacity = atomtable.emptybuckets = newsize;
    atomtable.totalitems = 0;
    atomtable.values = calloc(atomtable.capacity, sizeof(atom));
    for (int i=0;i<oldcap;i++){ //for each old bucket
      atom a = oldvals[i];
      while (a != NULL){ //for each element in bucket i
	int arrpos = a->hashcode & (atomtable.capacity - 1);
	atom oldnext = a->next;
	//inset into new table, update a->next
	a->next = atomtable.values[arrpos];
	atomtable.values[arrpos] = a;
	//advance to next element of old chain
	a = oldnext;
      }
    }
  }
}

static uint32_t hash_string(const char* str, int len){
  //32-bit FNV hash
  const uint32_t fnvprime = 16777619;
  uint32_t hashcode = 2166136261;
  for (int i=0;i<len;i++){
    hashcode ^= ((unsigned char)*str++);
    hashcode *= fnvprime;
  }
  return hashcode;
}

atom atom_get(const char* string){
  return atom_get_len(string, strlen(string)+1);
}
atom atom_get_len(const char* string, int len){
  table_init();
  
  atom a = NULL;

  // Lookup in hashtable
  uint32_t hashcode = hash_string(string, len);
  int arrpos = hashcode & (atomtable.capacity-1);
  for (atom i = atomtable.values[arrpos]; i != NULL; i = i->next){
    if (len == i->length){
      if (!memcmp(string, i->string, len)){
	a = i;
	break;
      }
    }
  }
  if (!a){//not found, add to table
    a = malloc(sizeof(*a));
    a->string = malloc(len);
    memcpy(a->string, string, len);
    a->refcnt = 0;
    a->id = max_id++;
    a->length = len;
    a->hashcode = hashcode;

    a->next = atomtable.values[arrpos];
    atomtable.values[arrpos] = a;

    //update hashtable stats
    atomtable.totalitems++;
    if (!a->next){
      //this bucket was previously empty
      atomtable.emptybuckets--;
    }
    table_rehash();
  }
  a->refcnt++;
  return a;
}
atom atom_addref(atom a){
  a->refcnt++;
  return a;
}
void atom_decref(atom a){
  //FIXME: atom_decref is disabled for now, 
  //too much badly-written code isn't using addref
  //properly :P
  return;
  if (!a) return;
  table_init();
  a->refcnt--;
  assert(a->refcnt >= 0);
  if (a->refcnt <= 0){
    int arrpos = a->hashcode & (atomtable.capacity-1);
    assert(atomtable.values[arrpos] != NULL);
    // Remove it from position arrpos in hashtable
    if (atomtable.values[arrpos] == a){//first element
      atomtable.values[arrpos] = atomtable.values[arrpos]->next;
    }else{//not first element
      for (atom i = atomtable.values[arrpos]; i != NULL; i = i->next){
	if (i->next == a){
	  i->next = i->next->next;
	  break;
	}
      }
    }
    //update hashtable stats
    atomtable.totalitems--;
    if (atomtable.values[arrpos] == NULL){
      atomtable.emptybuckets++;
    }
    table_rehash();
    free(a->string);
    free(a);
  }
}

