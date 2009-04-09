#include <stdlib.h>
#include "bitops.h"
#include "error.h"

bitset_t bitset_alloc(int capacity) {
	bitset_t set = malloc(sizeof(struct bitset));
	
	// calculate number of int's needed to represent up to capacity items
	int num = (capacity / NBITS_INT) + 1;// + ((capacity % NBITS_INT) == 0 ? 0 : 1);
	set->capacity = capacity;
	set->data = malloc(num * NBITS_INT / NBITS_BYTE);
	
	// initialise to zeros
	int i;
	for(i = 0; i < num; ++i)
	{
		*(set->data + i) = 0;
	}
	
	return set;
}

void bitset_free(bitset_t b) {
	free(b->data);
	free(b);
}

int bitset_get(bitset_t b, int bit){
	if(bit < 0 || bit >= b->capacity)
	{
		// index out of bounds
		say(ASSERT,"index %d out of bounds (cap=%d) in bitset_get", bit, b->capacity);
		return -1;
	}
	
	int n_arr = bit / NBITS_INT;				// the offset into the array we wish to look at
								// bit % int_bits - the position in the int we wish to look at
	unsigned int bitmask = 1 << (bit % NBITS_INT);	// the bitmask of the bit we wish to look at
	
	return (bitmask & *(b->data + n_arr) ? 1 : 0);
}

void bitset_set(bitset_t b, int bit, int val) {
	if(bit < 0 || bit >= b->capacity)
	{
	  say(ASSERT,"index %d out of bounds (cap=%d) in bitset_set", bit, b->capacity);
	}
	
	int n_arr = bit / NBITS_INT;				// the position in the array we wish to look at
								// bit % int_bits - the position in the int we wish to look at
	unsigned int bitmask = 1 << (bit % NBITS_INT);	// the bitmask of the bit we wish to look at

	if (val) {
		*(b->data + n_arr) = *(b->data + n_arr) | bitmask;
	} else {
		*(b->data + n_arr) = *(b->data + n_arr) & (~bitmask);
	}	
}
