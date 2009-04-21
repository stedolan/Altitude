#ifndef _ALTITUDE_UTIL_H
#define _ALTITUDE_UTIL_H

/* Random utility functions */

/* DIV_ROUND_UP(a,b) for integers a,b is ceiling(a/b) */
/* b is evaluated more than once, it mustn't have side-effects */
#define DIV_ROUND_UP(a,b) ((a) + (b) - 1)/(b)


#endif
