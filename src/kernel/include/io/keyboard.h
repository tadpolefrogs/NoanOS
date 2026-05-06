#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "core/types.h"

void keyboard_init(void);
int keyboard_getchar(void);
void keyboard_flush(void);        /* Discard all buffered keypresses */
void keyboard_set_enabled(int);   /* 0 = stop buffering input, 1 = resume */
void keyboard_set_debug(int enable);
void keyboard_toggle_shift_lock(void);
void keyboard_toggle_symbol_mode(void);

#endif

