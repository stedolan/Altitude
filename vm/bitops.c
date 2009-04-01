#include "bitops.h"

struct bitset{
  /* FIXME: change to whatever type is handiest. If you need a well-defined 
     bit size, use uint32_t or whatever from stdint.h
  */
  int* bits;
  int capacity;
};
