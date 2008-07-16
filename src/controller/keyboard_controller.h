#ifndef _keyboard_controller_h
#define _keyboard_controller_h

#include "gui/window.h"
#include "songdata/songdata.h"
#include "config/config.h"

void keyboard_controller_init(wlist*, Config *);
int keyboard_controller_read_key(Window *);
void keyboard_controller_shutdown(void);
void keyboard_controller_check_timeout(void);

#endif /* keyboard_controller_h */