#include "memtypes.h"

/* A system function takes 4 params:
   - whether to store a return value
   - if the first arg is true, a userptr_t for the return value
   - how many parameters have been passed
   - the array of parameters
   
   it returns either true or false. If it returns false, the 
   user program is considered to have crashed.
*/
typedef int (*system_function)(int, userptr_t, int, primitive_val*);

extern const char* user_stdlib_names[];
extern const system_function user_stdlib_funcs[];
extern const int user_stdlib_count;
