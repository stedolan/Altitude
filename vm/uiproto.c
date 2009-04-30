#include "uiproto.h"
#include "atom.h"
#include "error.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "control.h"

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

int parse_command(struct program* current){
	if(atom_get("run") == atom_get(ui_proto_command_buf)) {
		altitude_run(current);
	}else if(atom_get("dump") == atom_get(ui_proto_command_buf)) {
		altitude_dump(current);
	}else if(atom_get("runback") == atom_get(ui_proto_command_buf)) {
		altitude_runback();
	}else if(atom_get("restart") == atom_get(ui_proto_command_buf)){
		altitude_restart();
	}else if(atom_get("view") == atom_get(ui_proto_command_buf)){
		altitude_view();
	}else if(atom_get("set") == atom_get(ui_proto_command_buf)){
		breakpoint_set(&ui_last_location);
	}else if(atom_get("unset") == atom_get(ui_proto_command_buf)){
		breakpoint_unset(&ui_last_location);
	}else if(atom_get("quit") == atom_get(ui_proto_command_buf)){
	        return 1;
	}
	else{
		sayf(BADCOMMAND, "Unrecognised command: %s", &ui_proto_command_buf);
	}
	return 0;
}
