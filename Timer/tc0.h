/*
 * tc0.h
 *
 * Created: 2015-08-09
 *  Author: Craig Hollinger
 *
 * Functions to access the control registers of Timer/Counter 0.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */ 

#ifndef _TC0_H_
#define _TC0_H_ 1

/* Timer/Counter 0 has 8 modes that determine the counting sequence of the
 * counter, the source for maximum (TOP) counter value, and the type of
 * waveform generated.  The table below determines the different modes.

 *  Mode | Operation  | TOP  |   Update  | TOV Flag
 *       |            |      |    OCRx   | set on
 *  ----------------------------------------------
 *   0   |   Normal   |  ff  | Immediate |  MAX
 *   1   | PWM, Phase |  ff  |    TOP    | BOTTOM
 *       |  correct   |      |           |
 *   2   |    CTC     | OCRA | Immediate |  MAX
 *   3   | Fast PWM   |  ff  |  BOTTOM   |  MAX
 *   4   | Reserved   |  -   |    -      |   -
 *   5   | PWM, Phase | OCRA |    TOP    | BOTTOM
 *       |  correct   |      |           |
 *   6   | Reserved   |  -   |    -      |
 *   7   | Fast PWM   | OCRA |  BOTTOM   |  TOP
 *
 * The WGM bits (lower 2 in TCCR0A, upper 1 in TCCR0B) need to be set
 * using the defines below.
 *
 * Upper bit in TCCR0B is set to 0.
 */
#define TC0_TCCR0A_M0_NORMAL       0
#define TC0_TCCR0A_M1_PWM_FF       1
#define TC0_TCCR0A_M2_CTC          2
#define TC0_TCCR0A_M3_FASTPWM_FF   3
/* Upper bit in TCCR0B is set to 1.
 */
#define TC0_TCCR0A_M5_PWM_OCRA     1
#define TC0_TCCR0A_M7_FASTPWM_OCRA 3

/* The behavior of the Compare Match Output pin A depends on the timer mode
 * selected above.  The tables below show the functionality of the pin but
 * the data direction must be set accordingly.
 *
 * Non-PWM Mode
 *  TC0 Mode | OC0A behavior
 * ----------------------------------------------------
 *    0      | Normal port operation, OC0A disconnected
 *    1, 5   | Toggle OC0A on Compare Match
 *    2      | Clear OC0A on Compare Match
 *    3, 7   | Set OC0A on Compare Match
 *
 * Fast PWM Mode
 *  TC0 Mode | OC0A behavior
 * ------------------------------------------------------------
 *    0, 1   | Normal port operation, OC0A disconnected.
 *    5      | Toggle OC0A on Compare Match.
 *    2      | Clear OC0A on Compare Match, set OC0A at BOTTOM,
 *           | (non-inverting mode).
 *    3, 7   | Set OC0A on Compare Match, clear OC0A at BOTTOM,
 *           | (inverting mode).
 *
 * Phase Correct PWM Mode
 *  TC0 Mode | OC0A behavior
 * -------------------------------------------------------
 *    0, 1   | 0 Normal port operation, OC0A disconnected.
 *    5      | Toggle OC0A on Compare Match.
 *    2      | Clear OC0A on Compare Match when up-counting. Set OC0A on
 *           | Compare Match when down-counting.
 *    3, 7   | Set OC0A on Compare Match when up-counting. Clear OC0A on
 *           |Compare Match when down-counting.
 *
 * The COM0A bits in TCCR0A need to be set using one of the defines
 * below.
 */
#define TC0_TCCR0A_OC0A_MODE0 0
#define TC0_TCCR0A_OC0A_MODE1 1
#define TC0_TCCR0A_OC0A_MODE2 2
#define TC0_TCCR0A_OC0A_MODE3 3

/* The behavior of the Compare Match Output pin B depends on the timer mode
 * selected above.  The tables below show the functionality of the pin but
 * the data direction must be set accordingly.
 *
 * Non-PWM Mode
 *  TC0 Mode | OC0B behavior
 * ----------------------------------------------------
 *    0      | Normal port operation, OC0B disconnected.
 *    1, 5   | Toggle OC0B on Compare Match
 *    2      | Clear OC0B on Compare Match
 *    3, 7   | Set OC0B on Compare Match
 *
 * Fast PWM Mode
 *  TC0 Mode | OC0B behavior
 * ------------------------------------------------------------
 *    0      | Normal port operation, OC0B disconnected.
 *    1, 5   | Reserved
 *    2      | Clear OC0B on Compare Match, set OC0B at BOTTOM,
 *           | (non-inverting mode).
 *    3, 7   | Set OC0B on Compare Match, clear OC0B at BOTTOM,
 *           | (inverting mode).
 *
 * Phase Correct PWM Mode
 *  TC0 Mode | OC0B behavior
 * -------------------------------------------------------
 *    0      | 0 Normal port operation, OC0B disconnected.
 *    1, 5   | Reserved
 *    2      | Clear OC0B on Compare Match when up-counting. Set OC0B on
 *           | Compare Match when down-counting.
 *    3, 7   | Set OC0B on Compare Match when up-counting. Clear OC0B on
 *           |Compare Match when down-counting.
 *
 * The COM0A bits in TCCR0B need to be set using one of the defines
 * below.
 */
#define TC0_TCCR0A_OC0B_MODE0 0
#define TC0_TCCR0A_OC0B_MODE1 1
#define TC0_TCCR0A_OC0B_MODE2 2
#define TC0_TCCR0A_OC0B_MODE3 3

/* There are 8 clock sources for TC0. The clockSelect bits are set using one
 * of the defines below.
 */
#define TC0_TCCR0B_CLK_NONE      0 /* no source, clock is stopped */
#define TC0_TCCR0B_CLK_PRSC1     1 /* no prescale */
#define TC0_TCCR0B_CLK_PRSC8     2 /* prescale divide by 8 */
#define TC0_TCCR0B_CLK_PRSC64    3 /* prescale divide by 64 */
#define TC0_TCCR0B_CLK_PRSC256   4 /* prescale divide by 256 */
#define TC0_TCCR0B_CLK_PRSC1024  5 /* prescale divide by 1024 */
#define TC0_TCCR0B_CLK_EXTFALL   6 /* external source clock on falling edge */
#define TC0_TCCR0B_CLK_EXTRISE   7 /* external source clock on rising edge */

/* Timer 0 Control Register A. */
typedef union {
  struct {
    uint8_t wgm0l : 2;/* waveform generation */
    uint8_t : 2;
    uint8_t com0b : 2;/* compare match output B */
    uint8_t com0a : 2;/* compare match output A */
  };
  uint8_t reg;
} TC0_TCCR0A_type;

/* Timer 0 Control Register B. */
typedef union {
  struct {
    uint8_t cs0 : 3;/* clock source select */
    uint8_t wgm0h : 1;/* upper bit of waveform generation */
    uint8_t : 2;
    uint8_t foc0b : 1;/* force output compare B */
    uint8_t foc0a : 1;/* force output compare A */
  };
  uint8_t reg;
} TC0_TCCR0B_type;

/* Timer 0 Timer/Counter register. */
typedef uint8_t TC0_TCNT0_type;// TCNT0

/* Timer 0 Output Compare A register. */
typedef uint8_t TC0_OCR0A_type;// OCR0A

/* Timer 0 Output Compare B register. */
typedef uint8_t TC0_OCR0B_type;// TCNT0

/* Timer 0 Interrupt Enable register. */
typedef union {
  struct {
    uint8_t toie0 : 1;/* timer overflow interrupt enable */
    uint8_t ocie0a : 1;/* output compare A interrupt enable */
    uint8_t ocie0b : 1;/* output compare B interrupt enable */
    uint8_t : 5;
  };
  uint8_t reg;  
} TC0_TIMSK0_type;

/* Timer 0 Interrupt Flag register. */
typedef union {
  struct {
    uint8_t tov0 : 1;/* timer overflow interrupt flag */
    uint8_t ocf0a : 1;/* output compare A interrupt flag */
    uint8_t ocf0b : 1;/* output compare B interrupt flag */
    uint8_t : 5;
  };
  uint8_t reg;
} TC0_TIFR0_type;

/* Timer 0 control registers data structure. */
typedef struct {
  TC0_TCCR0A_type tccr0a;
  TC0_TCCR0B_type tccr0b;
  TC0_TCNT0_type  tcnt0;
  TC0_OCR0A_type  ocr0a;
  TC0_OCR0B_type  ocr0b;
  TC0_TIMSK0_type timsk0;
  TC0_TIFR0_type  tifr0;
} Timer_Counter0_TYPE;

/* Fill a data structure with the contents of the Timer/Counter 0 registers. */
void tc0_get_config(Timer_Counter0_TYPE *timer);

/* Write the data structure back to the Timer/Counter 0 registers. */
void tc0_set_config(Timer_Counter0_TYPE *timer);

/* Write the value to the OCR0A and OCR0B registers.  */
void tc0_set_ocr0a(uint8_t val);
void tc0_set_ocr0b(uint8_t val);

/* Return the value of the OCR0A and OCR0B registers. */
uint8_t tc0_get_ocr0a(void);
uint8_t tc0_get_ocr0b(void);

#endif /* _TC0_H_ */
