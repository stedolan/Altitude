#ifndef _ALTITUDE_UI_PROTO_H
#include "uiproto.h"
#endif
#ifndef _ALTITUDE_ATOM_H
#include "atom.h"
#endif
#ifndef _ALTITUDE_ERROR_H
#include "error.h"
#endif
#include "stdio.h"
#include "stdlib.h"

void get_command(){
  int __a_raptor_is_you = scanf("%s", ui_proto_command_buf);
  if(strncmp("set\0", ui_proto_command_buf, __a_raptor_is_you) == 0){
     int line_number=0;
     scanf("%d", &line_number);
     ui_last_location.filename = (current_location()).filename;
     ui_last_location.line = line_number;
     ui_last_location.bytepos = 0;
  }
  if(strncmp("unset\0", ui_proto_command_buf, __a_raptor_is_you) == 0){
     int line_number=0;
     scanf("%d", &line_number);
     ui_last_location.filename = (current_location()).filename;
     ui_last_location.line = line_number;
     ui_last_location.bytepos = 0;
  }
}

void parse_command(){
  if(strncmp(ui_proto_command_buf, "run\0", 3) == 0){
    altitude_run();
  }else if(strncmp(ui_proto_command_buf, "runback\0", 7) == 0){
    altitude_runback();
  }else if(strncmp(ui_proto_command_buf, "restart\0", 7) == 0){
    altitude_restart();
  }else if(strncmp(ui_proto_command_buf, "view\0", 4) == 0){
    altitude_view();
  }else if(strncmp(ui_proto_command_buf, "set\0", 3) == 0){
    breakpoint_set(&ui_last_location);
  }else if(strncmp(ui_proto_command_buf, "unset\0", 5) == 0){
    breakpoint_unset(&ui_last_location);
  }else{
    say(E_BADCOMMAND, "Unrecognised command");
  }
}
