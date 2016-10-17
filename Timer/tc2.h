/*
 * tc2.h
 *
 * Created: 2016-08-27
 *  Author: Craig Hollinger
 *
 * Functions to access the control registers of Timer/Counter 2.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */ 

#ifndef _TC2_H_
#define _TC2_H_ 1

/* Timer/Counter 2 has 8 modes that determine the counting sequence of the
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
 * The WGM bits (lower 2 in TCCR2A, upper 1 in TCCR2B) need to be set
 * using the defines below.
 *
 * Upper bit in TCCR2B is set to 0.
 */
#define TC2_TCCR2A_M0_NORMAL       0
#define TC2_TCCR2A_M1_PWM_FF       1
#define TC2_TCCR2A_M2_CTC          2
#define TC2_TCCR2A_M3_FASTPWM_FF   3
/* Upper bit in TCCR2B is set to 1.
 */
#define TC2_M5_PWM_OCRA     1
#define TC2_M7_FASTPWM_OCRA 3

/* The behavior of the Compare Match Output pin A depends on the timer mode
 * selected above.  The tables below show the functionality of the pin but
 * the data direction must be set accordingly.
 *
 * Non-PWM Mode
 *  TC2 Mode | OC2A behavior
 * ----------------------------------------------------
 *    0      | Normal port operation, OC2A disconnected
 *    1, 5   | Toggle OC2A on Compare Match
 *    2      | Clear OC2A on Compare Match
 *    3, 7   | Set OC2A on Compare Match
 *
 * Fast PWM Mode
 *  TC2 Mode | OC2A behavior
 * ------------------------------------------------------------
 *    0, 1   | Normal port operation, OC2A disconnected.
 *    5      | Toggle OC2A on Compare Match.
 *    2      | Clear OC2A on Compare Match, set OC2A at BOTTOM,
 *           | (non-inverting mode).
 *    3, 7   | Set OC2A on Compare Match, clear OC2A at BOTTOM,
 *           | (inverting mode).
 *
 * Phase Correct PWM Mode
 *  TC0 Mode | OC2A behavior
 * -------------------------------------------------------
 *    0, 1   | 0 Normal port operation, OC2A disconnected.
 *    5      | Toggle OC2A on Compare Match.
 *    2      | Clear OC2A on Compare Match when up-counting. Set OC2A on
 *           | Compare Match when down-counting.
 *    3, 7   | Set OC2A on Compare Match when up-counting. Clear OC2A on
 *           |Compare Match when down-counting.
 *
 * The COM2A bits in TCCR2A need to be set using one of the defines
 * below.
 */
#define TC2_TCCR2A_OC2A_MODE0 0
#define TC2_TCCR2A_OC2A_MODE1 1
#define TC2_TCCR2A_OC2A_MODE2 2
#define TC2_TCCR2A_OC2A_MODE3 3

/* The behavior of the Compare Match Output pin B depends on the timer mode
 * selected above.  The tables below show the functionality of the pin but
 * the data direction must be set accordingly.
 *
 * Non-PWM Mode
 *  TC2 Mode | OC2B behavior
 * ----------------------------------------------------
 *    0      | Normal port operation, OC2B disconnected.
 *    1, 5   | Toggle OC2B on Compare Match
 *    2      | Clear OC2B on Compare Match
 *    3, 7   | Set OC2B on Compare Match
 *
 * Fast PWM Mode
 *  TC2 Mode | OC2B behavior
 * ------------------------------------------------------------
 *    0      | Normal port operation, OC2B disconnected.
 *    1, 5   | Reserved
 *    2      | Clear OC2B on Compare Match, set OC2B at BOTTOM,
 *           | (non-inverting mode).
 *    3, 7   | Set OC2B on Compare Match, clear OC2B at BOTTOM,
 *           | (inverting mode).
 *
 * Phase Correct PWM Mode
 *  TC2 Mode | OC2B behavior
 * -------------------------------------------------------
 *    0      | 0 Normal port operation, OC2B disconnected.
 *    1, 5   | Reserved
 *    2      | Clear OC2B on Compare Match when up-counting. Set OC2B on
 *           | Compare Match when down-counting.
 *    3, 7   | Set OC2B on Compare Match when up-counting. Clear OC2B on
 *           |Compare Match when down-counting.
 *
 * The COM2A bits in TCCR2B need to be set using one of the defines
 * below.
 */
#define TC2_TCCR2A_OC2B_MODE0 0
#define TC2_TCCR2A_OC2B_MODE1 1
#define TC2_TCCR2A_OC2B_MODE2 2
#define TC2_TCCR2A_OC2B_MODE3 3

/* There are 8 clock sources for TC2. The clockSelect bits are set using one
 * of the defines below.
 */
#define TC2_TCCR2B_CLK_NONE      0 /* no source, clock is stopped */
#define TC2_TCCR2B_CLK_PRSC1     1 /* no prescale divide by 1*/
#define TC2_TCCR2B_CLK_PRSC8     2 /* prescale divide by 8 */
#define TC2_TCCR2B_CLK_PRSC32    3 /* prescale divide by 32 */
#define TC2_TCCR2B_CLK_PRSC64    4 /* prescale divide by 64 */
#define TC2_TCCR2B_CLK_PRSC128   5 /* prescale divide by 128 */
#define TC2_TCCR2B_CLK_PRSC256   6 /* prescale divide by 256 */
#define TC2_TCCR2B_CLK_PRSC1024  7 /* prescale divide by 1024 */

/* Timer 2 Control Register A. */
typedef union {
  struct {
    uint8_t wgm2l : 2;/* waveform generation */
    uint8_t : 2;
    uint8_t com2b : 2;/* compare match output B */
    uint8_t com2a : 2;/* compare match output A */
  };
  uint8_t reg;
} TC2_TCCR2A_type;

/* Timer 2 Control Register B. */
typedef union {
  struct {
    uint8_t cs2 : 3;/* clock source select */
    uint8_t wgm2h : 1;/* upper bit of waveform generation */
    uint8_t : 2;
    uint8_t foc2b : 1;/* force output compare B */
    uint8_t foc2a : 1;/* force output compare A */
  };
  uint8_t reg;
} TC2_TCCR2B_type;

/* Timer 2 Timer/Counter register. */
typedef uint8_t TC2_TCNT2_type;

/* Timer 2 Output Compare A register. */
typedef uint8_t TC2_OCR2A_type;

/* Timer 2 Output Compare B register. */
typedef uint8_t TC2_OCR2B_type;

/* Timer 2 Interrupt Enable register. */
typedef union {
  struct {
    uint8_t toie2 : 1;/* timer overflow interrupt enable */
    uint8_t ocie2a : 1;/* output compare A interrupt enable */
    uint8_t ocie2b : 1;/* output compare B interrupt enable */
    uint8_t : 5;
  };
  uint8_t reg;  
} TC2_TIMSK2_type;

/* Timer 2 Interrupt Flag register. */
typedef union {
  struct {
    uint8_t tov2 : 1;/* timer overflow interrupt flag */
    uint8_t ocf2a : 1;/* output compare A interrupt flag */
    uint8_t ocf2b : 1;/* output compare B interrupt flag */
    uint8_t : 5;
  };
  uint8_t reg;
} TC2_TIFR2_type;

/* Timer 2 control registers data structure. */
typedef struct {
  TC2_TCCR2A_type tccr2a;
  TC2_TCCR2B_type tccr2b;
  TC2_TCNT2_type  tcnt2;
  TC2_OCR2A_type  ocr2a;
  TC2_OCR2B_type  ocr2b;
  TC2_TIMSK2_type timsk2;
  TC2_TIFR2_type  tifr2;
} Timer_Counter2;

/* Fill a data structure with the contents of the Timer/Counter 2 registers. */
void tc2_get_config(Timer_Counter2 timer);

/* Write the data structure back to the Timer/Counter 2 registers. */
void tc2_set_config(Timer_Counter2 timer);

/* Write the value to the OCR2A and OCR2B registers.  */
void tc2_set_ocr2a(uint8_t val);
void tc2_set_ocr2b(uint8_t val);

/* Return the value of the OCR0A and OCR2B registers. */
uint8_t tc2_get_ocr2a(void);
uint8_t tc2_get_ocr2b(void);

#endif /* _TC2_H_ */