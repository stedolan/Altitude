#include "uiproto.h"
#include "atom.h"
#include "error.h"
#include "stdio.h"
#include "stdlib.h"

char ui_proto_command_buf[1024]; //scratch space for command data
struct location ui_last_location; //anytime a location is parsed, it goes in here

void get_command(){
  int __a_raptor_is_you = scanf("%s", ui_proto_command_buf);
  if(strncmp("set", ui_proto_command_buf, __a_raptor_is_you) == 0){
     int line_number=0;
     scanf("%d", &line_number);
     ui_last_location.filename = (current_location()).filename;
     ui_last_location.line = line_number;
     ui_last_location.bytepos = 0;
  }
  if(strncmp("unset", ui_proto_command_buf, __a_raptor_is_you) == 0){
     int line_number=0;
     scanf("%d", &line_number);
     ui_last_location.filename = (current_location()).filename;
     ui_last_location.line = line_number;
     ui_last_location.bytepos = 0;
  }
}

void parse_command(){
	if(get_atom("run") == get_atom(ui_proto_command_buf)) {
		altitude_run();
	} else if(get_atom("runback") == get_atom(ui_proto_command_buf)) {
		altitude_runback();
	}else if(get_atom("restart") == get_atom(ui_proto_command_buf)){
		altitude_restart();
	}else if(get_atom("view") == get_atom(ui_proto_command_buf)){
		altitude_view();
	}else if(get_atom("set") == get_atom(ui_proto_command_buf)){
		breakpoint_set(&ui_last_location);
	}else if(get_atom("unset") == get_atom(ui_proto_command_buf)){
		breakpoint_unset(&ui_last_location);
	}else{
		sayf(E_BADCOMMAND, "Unrecognised command: %s", &ui_proto_command_buf);
	}
}
