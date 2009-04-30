#ifndef _ALTITUDE_UI_PROTO_H
#define _ALTITUDE_UI_PROTO_H
#include "error.h"
#include "bytecode.h"

void get_command();
int parse_command(struct program*);

#endif
