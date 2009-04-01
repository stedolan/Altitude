#ifndef _ALTITUDE_BITOPS_H
#define _ALTITUDE_BITOPS_H
/* Bit operations, used in various places */

/* x & NBIT_MASK(n) is the low n bits of x */
#define NBIT_MASK(n) ((1<<n) - 1)


/* bitset datatype */

typedef struct bitset *bitset_t;


/* Allocate a bitset, with the specified capacity */
bitset_t bitset_alloc(int capacity);

/* Deallocate any memory used for the bitset */
void bitset_free(bitset_t b);

/* Get a bit. The bit must be >=0 and <capacity */
int bitset_get(bitset_t b, int bit);

/* Set a bit. The bit must be >=0 and <capacity */
void bitset_set(bitset_t b, int bit, int val);



#endif
