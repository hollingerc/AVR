/*
 * pbmxkeys.c
 *
 * Created: 2015-08-09
 *  Author: Craig Hollinger
 *
 * These functions process up to four momentary contact pushbutton keys that
 * are multiplexed with another device.  This device must use the IO pins as
 * output only.  The function pbkeys_run() is called at regular intervals
 * and the IO pins are switched from output to input to look for a pressed key.
 * Prior to the function returning, the IO pins are switched back to output.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */ 

#include  <avr/io.h>
#include  <keypad/pbmxkeys.h>

/*
 These are the states the keypad process can be in.
*/

enum{ PBKeyNotProcess,        /* no PB keys are being processed */
      PBKeyClsdNotDbncd,      /* PB key is closed, not debounced */
      PBKeyPressed,           /* PB key is pressed and debounced */
      PBKeyHeld,              /* PB key is pressed past hold timeout */
      PBKeyPrsdOpenNotDbncd,  /* PB key that was pressed is open, not debounced */
      PBKeyHeldOpenNotDbncd   /* PB key that was held is open, not debounced */
};

/*
 This sets the key held time-out.  One second works well.  This assumes that 
 pb_keys() is called every 10ms.
*/

#define PBKEYHOLDTIME 100

volatile uint8_t *PBKeysPIN,
                 *PBKeysDDR;
unsigned char PBKeysMask,
              PBKeyState,
              PBKeyHeldTimer;

/*
 These two bytes hold the key pressed and held flags for each of the four keys.
 The upper or lower nibble will be used depending on which half of the IO port
 is used.
*/

volatile uint8_t PBKeyHeldFlgs;
volatile uint8_t PBKeyPrsdFlgs;

/*
 * pbmxkeys_init()
 *
 * Initialize the pushbutton key process.  The address of the I/O port where
 * the four key are connected is passed in the first parameter.  A mask
 * that shows which nibble is in the second parameter.
 */
void pbmxkeys_init(volatile uint8_t *keyPort, unsigned char keyMask)
{

  PBKeysDDR = keyPort - 1;
  PBKeysPIN = keyPort - 2;
  PBKeysMask = keyMask;
  PBKeyState = PBKeyNotProcess;
  PBKeyHeldTimer = PBKEYHOLDTIME;
  PBKeyHeldFlgs = 0;
  PBKeyPrsdFlgs = 0;

}/* end pbmxkeys_init() */

/*
 * pbkeys_run()
 *
 * Detect and debounce four pushbutton keys.  The keys are connected to a single
 * 8-bit IO port.  The port is shared with an output device and must be changed
 * to input to detect the keys.  After the port is tested for keys pressed, it 
 * is changed back to output.
 *
 * This routine expects to be called at regular intervals, 10msec is assumed.
 * The interval (scan rate) is used to debounce the keys.
 *
 * When a pushbutton key is pressed and released, the corresponding Pressed flag
 * is set.  If a pushbutton key is pressed and held longer than 1 second, the 
 * corresponding Held flag is set.  This routine only sets the flags, routines
 * that use the flags will need to clear them.  The flags are stored in 
 * PBKeyHeldFlgs and PBKeyPrsdFlgs
 *
 * This routine assumes that it is called every 20ms and is state driven.  There
 * are 6 states:
 *
 *       0. no pushbutton keys are being processed
 *       1. a pushbutton key was detected closed, but not yet debounced.
 *       2. a pushbutton key has been pressed
 *       3. a pressed pushbutton key was held past the hold time
 *       4. a pressed pushbutton key was released but not debounced 
 *       5. a held pushbutton key was released but not debounced
 *
 * The Pressed flag will be set after the corresponding pushbutton key has been 
 * released and debounced open.  This occurs at the end of state 2.  The key 
 * Held flag will be set after the corresponding pushbutton key is held closed 
 * for more than 1 second.  This occurs at the end of state 3.
 *
 * The key Held time is determined by the value in PBKEYHOLDTIME and the 
 * frequency which this routine is called.
 */
void  pbmxkeys_run(void)
{

  static uint8_t PBKeyCode;

  *PBKeysDDR &= ~PBKeysMask; /* make the key pins input */
  if (PBKeyState) /* is there already a key being processed? */
  {

    switch (PBKeyState) /* there is a key being processed, test the state */
    {

/****** PBKeyState = _PBKeyClsdNotDbncd (1) ******/

    case PBKeyClsdNotDbncd:
/*
 A pushbutton key has been detected pressed but not yet debounced.  

 Determine if the pushbutton key is still pressed.  If it has been released, 
 change PBKeyState to _PBKeyNotProcess to go back to the no keys being 
 processed state.  If it is still pressed, change PBKeystate to _PBKeyPressed
 to process the debounce.
*/
      if((~(*PBKeysPIN) & PBKeysMask) == PBKeyCode)
      {
        PBKeyState = PBKeyPressed;  /* the key was pressed */
      }
      else
      {
        PBKeyState = PBKeyNotProcess; /* the key was not pressed */
      }/* end if((~(*PBKeysPIN) & PBKeysMask) == PBKeyCode) */

    break;

/****** PBKeyState = _PBKeyPressed (2) ******/

    case PBKeyPressed:
/*
 Determine if the debounced pushbutton key is still closed.  If it is open, 
 change the state to _KeyPrsdOpenNotDbncd to start the debounce open process.

 As long as the key remains closed, the hold timer is decremented.  When the 
 timer expires, reset it and set the corresponding key held flag.
*/
      if((~(*PBKeysPIN) & PBKeysMask) == PBKeyCode) /* is key still held? */
      {
        if(--PBKeyHeldTimer == 0) /* yes, decrement held timer, has it timed out? */
        {
          PBKeyHeldTimer = PBKEYHOLDTIME;
          PBKeyState = PBKeyHeld;
          PBKeyHeldFlgs |= PBKeyCode; /* set the key held flag */
        } // end if(--PBKeyHeldTimer == 0)
      } // end if((~(*PBKeysPIN) & PBKeysMask) == PBKeyCode)
      else
      {
        PBKeyState = PBKeyPrsdOpenNotDbncd;
      }

      break;

/****** KeyState = _PBKeyHeld (3) ******/

    case PBKeyHeld:
/*
 The pushbutton key hold timer has expired.

 Determine if the pushbutton key is still pressed.  If it is, do nothing.  If
 the key is open, start the debounce process.
*/
      if (!((~(*PBKeysPIN) & PBKeysMask) == PBKeyCode))
      {
        PBKeyState = PBKeyHeldOpenNotDbncd;
      }

      break;

/****** PBKeyState = _PBKeyPrsdOpenNotDbncd (4) ******/

      case PBKeyPrsdOpenNotDbncd:
/*
 The pushbutton key has been released before the hold timer has expired but
 not yet debounced.  If this state is reached with the key released, it now 
 becomes debounced.

 Determine if the pushbutton key is closed.  If it is, do nothing.  Otherwise,
 reset hold timer, change state to no key being processed and set the corresponding
 key flag.
*/
      if(!((~(*PBKeysPIN) & PBKeysMask) == PBKeyCode))
      {
        PBKeyHeldTimer = PBKEYHOLDTIME;
        PBKeyState = PBKeyNotProcess;
        PBKeyPrsdFlgs |= PBKeyCode;
      }/* end if(!((~(*PBKeysPIN) & PBKeysMask) == PBKeyCode)) */

      break;

/****** PBKeyState = _PBKeyHeldOpenNotDbncd (5) ******/

      case PBKeyHeldOpenNotDbncd:
/*
 The pushbutton key has been released after hold timer has expired but
 not yet debounced.

 Determine if the pushbutton key is closed.  If it is, do nothing.  Otherwise, end
 the key process and change the state.
*/
      if ((~(*PBKeysPIN) & PBKeysMask) == PBKeyCode)
      {
        break;
      }
      else
      {
        PBKeyState = PBKeyNotProcess;
      }

      break;

    }  /* end switch (PBKeyState) */
  } /* end if (PBKeyState) */

// A key is not being processed. Read the port to see if one is pressed.
  else
  {
    if (~(*PBKeysPIN) & PBKeysMask)
    {
      PBKeyCode = (~(*PBKeysPIN) & PBKeysMask); /* Save code from key port. */
      PBKeyState = PBKeyClsdNotDbncd; /* Start the debounce process. */
    } /* end else if (PBKeyState) */
  }
  *PBKeysDDR |= PBKeysMask;  /* make key pins output */

} /* end pbmxkeys_run() */

/*
 * pbmxkeys_clear()
 *
 * Clear all pushbutton key flags.
 */
void pbmxkeys_clear(void)
{

  PBKeyHeldFlgs = 0;
  PBKeyPrsdFlgs = 0;

}/* end pbmxkeys_clear() */

/*
 * pbmxkeys_get_flags()
 *
 * Return the pressed and held flags.  The held flags are in the upper nibble,
 * the pressed flags are in the lower nibble.
 */
unsigned char pbmxkeys_get_flags(void)
{
  unsigned char flags = 0;

  if(PBKeysMask == PBKEYS_LOWER)
  {
    flags = (PBKeyPrsdFlgs & PBKEYS_LOWER);
    flags |= (PBKeyHeldFlgs << 4);
  }
  else
  {
    if(PBKeysMask == PBKEYS_UPPER)
    {
      flags = (PBKeyHeldFlgs & PBKEYS_UPPER);
      flags |= (PBKeyPrsdFlgs >> 4);
    }/* end if(PBKeysMask == PBKEYS_UPPER) */
  }/* end if(PBKeysMask == PBKEYS_LOWER) */

  return(flags);

}/* end pbmxkeys_get_flags() */
