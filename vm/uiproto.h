#ifndef _ALTITUDE_UI_PROTO_H
#define _ALTITUDE_UI_PROTO_H
#include "error.h"

char ui_proto_command_buf[1024];//scratch space for command data
struct location ui_last_location;//anytime a location is parsed, it goes in here

void get_command();
void parse_command();

#endif
