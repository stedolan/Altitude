#ifndef _ALTITUDE_CONTROL_H
#define _ALTITUDE_CONTROL_H

#ifndef _ALTITUTDE_BYTECODE_H
#include "bytecode.h"
#endif //for struct program
/*
  This file contains the command functions called by the UI layer.
  unless explicitly specified, everything that writes data should write it
  to stdout. no reading need be done, input and parsing is handled
  by the ui protocol code
*/
void altitude_run(struct program*);
void altitude_dump(struct program*);
void altitude_runback();
void altitude_restart();
void altitude_view();

void breakpoint_set(struct location*);
void breakpoint_unset(struct location*);

#endif
