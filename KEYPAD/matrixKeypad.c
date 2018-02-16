/*
 * matrixKeypad.c
 *
 * Created: 2017-09-27
 * Author : Craig Hollinger
 *
 * This file contains a driver for reading the keys of a matrix keypad.  Up to
 * a user defined number keys can be processed for pressed and held keys.  A
 * maximum of 64 keys could be detected and processed.  The number of keys to
 * process is set by modifying MATRIX_KEYPAD_MAX_KEYS in the header file.
 *
 * The column lines are driven while the row lines are read to determine which
 * key(s) is(are) pressed.  This software can detect multiple key presses.
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
#include <avr/io.h>
#include "keypad/matrixKeypad.h"

/* flags to indicate detected keys */
#define KEY_PRESSED_FLAG (0b00000001)
#define KEY_HELD_FLAG    (0b00000010)
#define KEY_CHANGED_FLAG (0b00000100)

/* This structure holds the data for each key.
 *
 * flags        : holds the PRESSED and HELD flags
 * state        : holds the state of the key
 * code         : holds a unique code for the key, can be an ASCII character
 * debounceTimer: holds the debounce time in milliseconds
 * holdTimer    : holds the key hold time in milliseconds
 */
typedef struct
{
  uint8_t flags;
  uint8_t state;
  uint8_t code;
  uint8_t debounceTimer;
  uint16_t holdTimer;

} KEY_STRUCT_TYPE;

/* **** Local variables **** */

/* an array of key structures */
KEY_STRUCT_TYPE key[MATRIX_KEYPAD_MAX_KEYS];

/* pointers to the IO ports the rows and columns are connected to */
volatile uint8_t *rowPORT, *colPORT, *rowDDR, *colDDR, *rowPIN;

/* bit masks indicating which port pins are connected to a row or column */
uint8_t rowMask, colMask;

/* number of rows and columns in the matrix */
uint8_t numRows, numCols;

/* key debounce time, used to reset each key's debounce timer */
uint8_t keyDebounceTime = DEBOUNCE_TIME;

/* key hold time, used to reset each key's hold timer */
uint16_t keyHoldTime = HOLD_TIME;

/*
 * matrix_keypad)init()
 *
 * Initialize the keypad scanning process.  Setup the IO ports, initialize 
 * global variables, initialize the variables for each of the keys.
 *
 * rowPt, colPt:   pointers to the IO ports where keypad row and column lines
 *                 are connected
 * rowMsk, colMsk: set bits indicate which IO pins are connected to a keypad
 *                 row or column
 * keyCodes:       pointer to an array of unique codes, one for each key
 * rows, cols:     number of rows and columns
 */
void matrix_keypad_init(volatile uint8_t *rowPt, uint8_t rowMsk,
                        volatile uint8_t *colPt, uint8_t colMsk,
                        char *keyCodes,
                        uint8_t rows, uint8_t cols)
{
  uint8_t i;

/* setup pointers to row and column DDR and PIN registers */
  rowPORT = rowPt;
  colPORT = colPt;
  rowDDR = rowPt - 1;
  colDDR = colPt - 1;
  rowPIN = rowPt - 2;

/* initialize number of rows and columns */
  numRows = rows;
  numCols = cols;

/* initialize row and column masks */
  rowMask = rowMsk;
  colMask = colMsk;

/* The rows will be read while the columns are driven low one at a time.  Setup
   the row pins to be inputs with pull-ups enabled. */
  *rowDDR &= ~rowMsk;
  *rowPORT |= rowMsk; /* enable internal pull up resistors */

/* initialize the variables for each of the keys */
  for(i = 0; i < (cols * rows); i++)
  {
    key[i].flags = 0;
    key[i].state = KEYPAD_IDLE;
    key[i].code = keyCodes[i];
    key[i].debounceTimer = DEBOUNCE_TIME;
    key[i].holdTimer = HOLD_TIME;
  }/* end for(i = 0; i < (numCols * numRows); i++) */

}/* end matrixKeypad_init() */

/*
 * scan_keys()
 *
 * Each time this is called the column driver pins are sequentially enabled to
 * drive a keypad column low.  The rows are sequentially read to determine which
 * key has been pressed or released.  The state of each key is determined and
 * the key pressed or held flags are set appropriately.  A flag is returned to
 * indicate if a key has been processed.
 */
uint8_t matrix_keypad_scan_keys(void)
{
  uint8_t row, col, /* loop counters for row/column pins */
          keyIndex, /* points to current key */
          validKey, /* indicates a key has been processed - pressed or held */
          curRowPin, /* current row pin */
          colMskTemp, rowMskTemp, /* holds row/column pin position */
          col_i, row_i; /* counters to prevent hanging the process */

  validKey = 0;
  colMskTemp = 1;
  col_i = 8;

  /* go through the columns turning on the IO pin and driving it low */
  for(col = 0; col < numCols; col++)
  {
    /* find next active keypad column */
    while((colMask & colMskTemp) == 0)
    {
      colMskTemp <<= 1;
      if(--col_i == 0)
      {
        return(0);
      }
    }/* end while((colMask & colMskTemp) == 0) */

    *colDDR |= colMskTemp; /* make column pin output */
    *colPORT &= ~colMskTemp; /* make column pin low */

    rowMskTemp = 1;
    row_i = 8;

    /* go through the rows reading the state of each */
    for(row = 0; row < numRows; row++)
    {
      /* find next active keypad row pin */
      while((rowMask & rowMskTemp) == 0)
      {
        rowMskTemp <<= 1;
        if(--row_i == 0)
        {
          return(0);
        }
      }/* end while((rowMask & rowMskTemp) == 0) */

      keyIndex = row * numCols + col;
      curRowPin = *rowPIN & rowMskTemp;

      /* **** Check the key state ****
       *
       * Determine what to do based on the state of the current Key.
       */
      switch(key[keyIndex].state)
      {
      /* **** KEY_IDLE ****
       *
       * Check if the key has been pressed.  If it has, change the state to
       * key pressed but not debounced (KEY_PRSD_NDBNC).  Otherwise do nothing.
       */
        case KEYPAD_IDLE:
          if(curRowPin == 0)
          {
            key[keyIndex].state = KEYPAD_PRSD_NDBNC;
            key[keyIndex].debounceTimer = keyDebounceTime;
          }/* end if((rowPort & rowMskTemp) == 0) */
          break;

       /* **** KEY_PRSD_NDBNC ****
        *
        * Check if the key is still pressed.  If it is, change the state to
        * key is pressed (KEY_PRESSED), set the key pressed flag and start the
        * hold timer.
        *
        * If the key is not still pressed, changed the state to key is idle
        * (KEY_IDLE).
        */
        case KEYPAD_PRSD_NDBNC:
          if(curRowPin == 0)
          {
            if(--key[keyIndex].debounceTimer == 0)
            {
              key[keyIndex].state = KEYPAD_PRESSED;
              key[keyIndex].flags |= (KEY_PRESSED_FLAG | KEY_CHANGED_FLAG);
              validKey = 1;
              key[keyIndex].holdTimer = keyHoldTime;
            }
          }
          else
          {
            key[keyIndex].state = KEYPAD_IDLE;
          }/* end if(curRowPin == 0) */
          break;

       /* **** KEY_PRESSED ****
        *
        * Check if the key is still pressed.  If it is, decrement the hold
        * timer.  If the hold timer times out change the state to key is held
        * (KEY_HELD), set the key held flag and reset the timer.
        *
        * Otherwise changed the state to key released but not debounced
        * (KEY_RLSD_NDBNC).
        */
        case KEYPAD_PRESSED:
          if(curRowPin == 0)
          {
            if(--key[keyIndex].holdTimer == 0)
            {
              key[keyIndex].state = KEYPAD_HELD;
              key[keyIndex].flags |= (KEY_HELD_FLAG | KEY_CHANGED_FLAG);
              validKey = 1;

            }/* end if(--key[keyIndex].holdTimer == 0) */
          }
          else
          {
            key[keyIndex].state = KEYPAD_RLSD_NDBNC;
            key[keyIndex].debounceTimer = keyDebounceTime;

          }/* end if(curRowPin == 0) */
          break;

       /* **** KEY_HELD ****
        *
        * Check if the key has been released.  If it has, changed the state to
        * (KEY_RLSD_NDBNC) and start the debounce timer.
        *
        * Otherwise, do nothing.
        */
        case KEYPAD_HELD:
          if(curRowPin != 0)
          {
            key[keyIndex].state = KEYPAD_RLSD_NDBNC;
            key[keyIndex].debounceTimer = keyDebounceTime;

          }/* end if(curRowPin != 0) */
          break;

       /* **** KEY_RLSD_NDBNC ****
        *
        * Check if a recently pressed or held key is still released.  If so
        * decrement the debounce timer.  Otherwise, do nothing.
        *
        * If the debounce timer times out change the state to idle (KEY_IDLE).
        */
        case KEYPAD_RLSD_NDBNC:
          if(curRowPin != 0)
          {
            if(--key[keyIndex].debounceTimer == 0)
            {
              key[keyIndex].state = KEYPAD_IDLE;
              key[keyIndex].flags |= KEY_CHANGED_FLAG;
            }
          }/* end if(curRowPin != 0) */
          break;

        default:
          key[keyIndex].state = KEYPAD_IDLE;
          break;

      }/* end switch(key[keyIndex].state) */

      rowMskTemp <<= 1; /* look for next active row */
      row_i--;

    }/* end for(row = 0; row < numRows; row++) */

    *colPORT |= colMskTemp;/* make column pin high */
    *colDDR &= ~colMskTemp;/* make port pin input */
    colMskTemp <<= 1; /* look for next active column */
    col_i--;

  }/* end for(col = 0; col < numCols; col++) */

  return(validKey);

}/* end matrix_keypad_scan_keys() */

/*
 * matrix_keypad_get_pressed_keys()
 *
 * Scan through the array of key structures looking at the key flags.  Return
 * a pointer to an array or string containing the codes for the most recently
 * pressed keys.  If none are pressed, the array or string will contain zero.
 *
 * The flags indicating pressed keys are cleared by this function.
 */
uint8_t matrix_keypad_get_pressed_keys(char * keyStr)
{
  uint8_t i, j, validKey;

  j = 0;
  validKey = 0;
  keyStr[0] = 0;

  for(i = 0; i < (numCols * numRows); i++)
  {
    if((key[i].flags & KEY_PRESSED_FLAG) != 0)
    {
      keyStr[j++] = key[i].code;
      key[i].flags &= ~KEY_PRESSED_FLAG;
      validKey = 1;
    }
  }/* end for(i = 0; i < (numCols * numRows); i++) */

  keyStr[j] = 0;

  return(validKey);

}/* end matrix_keypad_get_pressed_keys( */

/*
 * matrix_keypad_get_held_keys()
 *
 * Scan through the array of key structures looking at the key flags.  Return
 * a pointer to an array or string containing the codes for the most recently
 * held keys.  If none are held, the array or string will contain zero.
 *
 * The flags indicating held keys are cleared by this function.
 */
uint8_t matrix_keypad_get_held_keys(char * keyStr)
{
  uint8_t i, j, validKey;

  j = 0;
  validKey = 0;
  keyStr[0] = 0;

  for(i = 0; i < (numCols * numRows); i++)
  {
    if((key[i].flags & KEY_HELD_FLAG) != 0)
    {
      keyStr[j++] = key[i].code;
      key[i].flags &= ~KEY_HELD_FLAG;
      validKey = 1;
    }
  }/* end for(i = 0; i < (numCols * numRows); i++) */

  keyStr[j] = 0;
  return(validKey);

}/* end matrix_keypad_get_held_keys( */

/*
 * matrix_keypad_set_hold_time()
 *
 * Set the key hold time in multiples of the interval that
 * matrix_keypad_scan_keys() is called.
 */
void matrix_keypad_set_hold_time(uint16_t time)
{
  keyHoldTime = time;

}/* end  matrix_keypad_set_hold_time() */

/*
 * matrix_keypad_set_debounce_time()
 *
 * Set the key debounce time in multiples of the interval that
 * matrix_keypad_scan_keys() is called.
 */
void matrix_keypad_set_debounce_time(uint8_t time)
{
  keyDebounceTime = time;

}/* end matrix_keypad_set_debounce_time() */

/*
 * matrix_keypad_get_pressed_status()
 *
 * Fill the given string with the state of the keys.  Assumes that keyStr[] is
 * big enough to hold the states of all the keys.
 */
uint8_t matrix_keypad_get_status(char *keyStr)
{
  uint8_t i, changed;

  changed = 0;
  for(i = 0; i < (numCols * numRows); i++)
  {
    keyStr[i] = key[i].state;
    if((key[i].flags & KEY_CHANGED_FLAG) != 0)
    {
      key[i].flags &= ~KEY_CHANGED_FLAG;
      changed = 1;
    }
  }/* end for(i = 0; i < (numCols * numRows); i++) */

  return(changed);

}/* end matrix_keypad_get_pressed_status() */
