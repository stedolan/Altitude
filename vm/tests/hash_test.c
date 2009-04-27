/*
 * Test for atom code and atom->void* hashtable code
 */

#include "../hashtable.h"
#include "../atom.h"
#include <stdio.h>
#include <assert.h>

void print_table(ht_atom_t t){
  printf("Symbol table:\n");
  for (ht_atom_iter i = ht_atom_begin(t); ht_atom_hasmore(i); i = ht_atom_next(i)){
    printf("  %s: %s\n", ht_atom_getkey(i)->string, (char*)ht_atom_getvalue(i));
  }
  printf("\n");
}

int main(){
  char* symbols[] = {"foo","bar","baz","awesomename","notasymbol"};
  atom foasdf = atom_get("foasdf");
  assert(atom_get("foasdf") == foasdf);
  printf("foasdf refcnt: %d\n", foasdf->refcnt);
  atom_decref(foasdf);
  atom_decref(foasdf);
  ht_atom_t s = ht_atom_alloc();
  for (int i=0;i<(int)(sizeof(symbols)/sizeof(symbols[0]));i++){
    ht_atom_set(s, atom_get(symbols[i]), symbols[i]);
  }
  print_table(s);
  for (int i=0;i<(int)(sizeof(symbols)/sizeof(symbols[0]));i++){
    assert(symbols[i] == ht_atom_get(s, atom_get(symbols[i])));
  }
  ht_atom_set(s, atom_get("bar"), "some other value");
  ht_atom_del(s, atom_get("foo"));
  print_table(s);
  return 0;
}
