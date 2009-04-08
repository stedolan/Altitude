#ifndef _ALTITUDE_BITOPS_H
#define _ALTITUDE_BITOPS_H
/* Bit operations, used in various places */

/* Is the argument a power of two? Nice hack, see
   http://www.ugcs.caltech.edu/~wnoise/base2.html */
#define IS_POWER_OF_TWO(x) !((x) & ((x) - 1))

/* x & NBIT_MASK(n) is the low n bits of x */
#define NBIT_MASK(n) ((1<<n) - 1)

/* FIXME: need a way to get the number of bits
	  per byte on the system we're using...
	  for now, I use the following two defs:
*/

/* number of bits in a byte */
#define NBITS_BYTE 8

/* number of bits in an int */
#define NBITS_INT (sizeof(unsigned int) * NBITS_BYTE)

/* bitset datatype */

struct bitset{
  /* FIXME: change to whatever type is handiest. If you need a well-defined 
     bit size, use uint32_t or whatever from stdint.h
  */
  unsigned int* data;
  int capacity;
};

typedef struct bitset* bitset_t;


/* Allocate a bitset, with the specified capacity */
bitset_t bitset_alloc(int capacity);

/* Deallocate any memory used for the bitset */
void bitset_free(bitset_t b);

/* Get a bit. The bit must be >=0 and <capacity */
int bitset_get(bitset_t b, int bit);

/* Set a bit. The bit must be >=0 and <capacity */
void bitset_set(bitset_t b, int bit, int val);



#endif
