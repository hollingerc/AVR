/*
 * matrixKeypad.h
 *
 * Created: 2017-09-27
 * Author : Craig Hollinger
 *
 * This file contains the public interface for a driver for reading the keys
 * of a matrix keypad.  Up to a user determined number of keys can be processed
 * for pressed and held keys.  The column lines are driven while the row lines
 * are read to determine which key(s) is(are) pressed.  This software can
 * detect multiple key presses.
 *
 * Each key is identified by a user supplied code, could be ASCII or binary
 * codes.  If binary codes are used, the zero code cannot be used to identify a
 * key.  Two functions are used to return the codes of the active keys (pressed
 * or held) in a zero terminated string (array).
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */ 
#ifndef _MATRIXKEYPAD_H_
#define _MATRIXKEYPAD_H_ 1

/* maximum number of keys that can be processed */
#define MATRIX_KEYPAD_MAX_KEYS (16)

/* These times will be determined by how often the key scan process is called,
 * 1ms works but 10ms works too.
 */
/* Number of intervals to debounce a key, assumes 1ms refresh rate. */
#define DEBOUNCE_TIME (10)

/* Number of intervals to determine if a key is held, assumes 1ms refresh rate. */
#define HOLD_TIME (1000)

/* These are the states each of the keys can take. */
enum
{
  KEYPAD_IDLE, /* not being processed */
  KEYPAD_PRSD_NDBNC, /* pressed but not debounced yet */
  KEYPAD_PRESSED, /* debounced pressed */
  KEYPAD_HELD, /* debounced held */
  KEYPAD_RLSD_NDBNC, /* previously pressed/held now released but not debounced */
  MAX_KEYPAD_STATES
};

void matrix_keypad_init(volatile uint8_t *rowPt, uint8_t rowMsk,
                        volatile uint8_t *colPt, uint8_t colMsk,
                        char *keyCodes,
                        uint8_t rows, uint8_t cols);
uint8_t matrix_keypad_scan_keys(void);
uint8_t matrix_keypad_get_pressed_keys(char * keyStr);
uint8_t matrix_keypad_get_held_keys(char * keyStr);
void matrix_keypad_set_hold_time(uint16_t time);
void matrix_keypad_set_debounce_time(uint8_t time);
uint8_t matrix_keypad_get_status(char *keyStr);

#endif /* _MATRIXKEYPAD_H_ */
