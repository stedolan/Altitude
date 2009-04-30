#ifndef _ALTITUDE_CONTROL_H
#include "control.h"
#endif

#include "interpreter.h" //for run()
void altitude_run(struct program* p){
  run(p);
}

void altitude_dump(struct program* p){
  dump(p);
}

void altitude_runback(){
}

void altitude_restart(){
}

void altitude_view(){
}

void breakpoint_set(struct location*){
}

void breakpoint_unset(struct location*){
}

