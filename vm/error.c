#include "error.h"
#include <stdlib.h>

/* For the moment, everything is printed and nothing is fatal */
/* Ideally, this should be runtime-configurable */

int _should_say(message_category m){
  if (m){}
  return 1;
}

void _possibly_quit(message_category m){
  if (m){}
}
