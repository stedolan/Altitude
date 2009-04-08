/*
 * Test code for bitset
 * creates a bitset representing ASCII codes
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "../bitops.h"

void bitset_print(bitset_t set)
{
	int i;
	for(i = 0; i < set->capacity; ++i)
	{
		if(bitset_get(set, i))
		{
			printf("1");
		}
		else
		{
			printf("0");
		}
	}

	printf("\n");
}

/*
// simple test
int main() {
	bitset_t set = bitset_alloc(8);
	bitset_set(set, 3, 1);
	bitset_print(set);
}
*/

void print_bitset_chars(bitset_t set)
{
	int i;
	for(i = 0; i < set->capacity; ++i)
	{
		if(bitset_get(set, i))
		{
			printf("%c ", (char)i);
		}
	}

	printf("\n");
}

// ascii test
int main()
{
	// create a bitset capable of representing any ascii char
	bitset_t set = bitset_alloc(128);

	char str1[255];

	printf("\033[1;34mplease enter characters to set:\033[0m ");
	fgets(str1, 255, stdin);

	char* p = str1;
	while(*p != '\0' && *p != '\n')
	{
		bitset_set(set, (int)(*p), 1);
		++p;
	}

	printf("\n\033[1;34mbitset looks like:\n\033[1;36m");
	bitset_print(set);
	print_bitset_chars(set);

	printf("\033[1;34mplease enter characters to delete:\033[0m ");
	fgets(str1, 255, stdin);
	p = str1;
	while(*p != '\0' && *p != '\n')
	{
		bitset_set(set, (int)(*p), 0);
		++p;
	}

	printf("\n\033[1;34mbitset looks like:\n\033[1;36m");
	bitset_print(set);
	print_bitset_chars(set);

	printf("\033[0m\n");
	return 0;
}
