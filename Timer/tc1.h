/*
 * tc1.h
 *
 * Created: 2016-08-25
 *  Author: Craig Hollinger
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
 */ 

#ifndef _TC1_H_
#define _TC1_H_ 1

/* Timer/Counter 1 has 16 modes that determine the counting sequence of the
 * counter, the source for maximum (TOP) counter value, and the type of
 * waveform generated.  The table below determines the different modes.
 *
 * Mode | WGM13 | WGM12 | WGM11 | WGM10 | Timer/Counter Mode of |  TOP   | Update of | TOV1 Flag
 *      |       |       |       |       |       Operation       |        |  OCR1x at |  Set on
 *--------------------------------------------------------------------------------------------
 *  0   |   0   |   0   |   0   |   0   |       Normal          | 0xFFFF | Immediate | MAX
 *  1   |   0   |   0   |   0   |   1   | PWM, Phase Correct,   | 0x00FF |    TOP    | BOTTOM
 *      |       |       |       |       |        8-bit          |        |           |
 *  2   |   0   |   0   |   1   |   0   | PWM, Phase Correct,   | 0x01FF |    TOP    | BOTTOM
 *      |       |       |       |       |        9-bit          |        |           |
 *  3   |   0   |   0   |   1   |   1   | PWM, Phase Correct,   | 0x03FF |    TOP    | BOTTOM
 *      |       |       |       |       |       10-bit          |        |           |
 *  4   |   0   |   1   |   0   |   0   |         CTC           | OCR1A  | Immediate | MAX
 *  5   |   0   |   1   |   0   |   1   |   Fast PWM, 8-bit     | 0x00FF |  BOTTOM   | TOP
 *  6   |   0   |   1   |   1   |   0   |   Fast PWM, 9-bit     | 0x01FF |  BOTTOM   | TOP
 *  7   |   0   |   1   |   1   |   1   |  Fast PWM, 10-bit     | 0x03FF |  BOTTOM   | TOP
 *  8   |   1   |   0   |   0   |   0   |   PWM, Phase and      |  ICR1  |  BOTTOM   | BOTTOM
 *      |       |       |       |       |  Frequency Correct    |        |           |
 *  9   |   1   |   0   |   0   |   1   |   PWM, Phase and      |        |           |
 *      |       |       |       |       |  Frequency Correct    | OCR1A  |  BOTTOM   | BOTTOM
 * 10   |   1   |   0   |   1   |   0   |  PWM, Phase Correct   |  ICR1  |    TOP    | BOTTOM
 * 11   |   1   |   0   |   1   |   1   |  PWM, Phase Correct   | OCR1A  |    TOP    | BOTTOM
 * 12   |   1   |   1   |   0   |   0   |          CTC          |  ICR1  | Immediate | MAX
 * 13   |   1   |   1   |   0   |   1   |      (Reserved)       |   –    |     –     | –
 * 14   |   1   |   1   |   1   |   0   |       Fast PWM        |  ICR1  |  BOTTOM   | TOP
 * 15   |   1   |   1   |   1   |   1   |       Fast PWM        | OCR1A  |  BOTTOM   | TOP
 *
 * The WGM bits (lower 2 in TCCR1A, upper 2 in TCCR1B) need to be set
 * using the defines below.
 */
#define TC1_TCCR1A_MODE0  0 // 0b00000000
#define TC1_TCCR1A_MODE1  1 // 0b00000001
#define TC1_TCCR1A_MODE2  2 // 0b00000010
#define TC1_TCCR1A_MODE3  3 // 0b00000011
#define TC1_TCCR1A_MODE4  0 // 0b00000000
#define TC1_TCCR1A_MODE5  1 // 0b00000001
#define TC1_TCCR1A_MODE6  2 // 0b00000010
#define TC1_TCCR1A_MODE7  3 // 0b00000011
#define TC1_TCCR1A_MODE8  0 // 0b00000000
#define TC1_TCCR1A_MODE9  1 // 0b00000001
#define TC1_TCCR1A_MODE10 2 // 0b00000010
#define TC1_TCCR1A_MODE11 3 // 0b00000011
#define TC1_TCCR1A_MODE12 0 // 0b00000000
#define TC1_TCCR1A_MODE13 1 // 0b00000001
#define TC1_TCCR1A_MODE14 2 // 0b00000010
#define TC1_TCCR1A_MODE15 3 // 0b00000011

#define TC1_TCCR1B_MODE0  0 // 0b00000000
#define TC1_TCCR1B_MODE1  0 // 0b00000000
#define TC1_TCCR1B_MODE2  0 // 0b00000000
#define TC1_TCCR1B_MODE3  0 // 0b00000000
#define TC1_TCCR1B_MODE4  1 // 0b00001000
#define TC1_TCCR1B_MODE5  1 // 0b00001000
#define TC1_TCCR1B_MODE6  1 // 0b00001000
#define TC1_TCCR1B_MODE7  1 // 0b00001000
#define TC1_TCCR1B_MODE8  2 // 0b00010000
#define TC1_TCCR1B_MODE9  2 // 0b00010000
#define TC1_TCCR1B_MODE10 2 // 0b00010000
#define TC1_TCCR1B_MODE11 2 // 0b00010000
#define TC1_TCCR1B_MODE12 3 // 0b00011000
#define TC1_TCCR1B_MODE13 3 // 0b00011000
#define TC1_TCCR1B_MODE14 3 // 0b00011000
#define TC1_TCCR1B_MODE15 3 // 0b00011000

/* The behaviour of the Compare Match Output pin A depends on the timer mode
 * selected above.  The tables below show the functionality of the pin but
 * the data direction must be set accordingly.
 *
 * Non-PWM Mode
 *  COM1 | COM0 | OC0A/B behaviour
 * ----------------------------------------------------
 *    0  |   0  | Normal port operation, OC1A/B disconnected
 *    0  |   1  | Toggle OC1A/B on Compare Match
 *    1  |   0  | Clear OC1A/B on Compare Match
 *    1  |   1  | Set OC1A/B on Compare Match
 *
 * Fast PWM Mode
 *  COM1 | COM0 | OC1A behaviour
 * ------------------------------------------------------------
 *    0  |   0  | Normal port operation, OC1A disconnected.
 *    0  |   1  | Toggle OC1A on Compare Match.
 *    1  |   0  | Clear OC1A on Compare Match, set OC1A at BOTTOM,
 *       |      | (non-inverting mode).
 *    1  |   1  | Set OC1A on Compare Match, clear OC1A at BOTTOM,
 *       |      | (inverting mode).
 *
 * Phase/Frequency Correct PWM Mode
 *  COM1 | COM0 | OC1A behaviour
 * -------------------------------------------------------
 *    0  |   0  | 0 Normal port operation, OC1A disconnected.
 *    0  |   1  | Toggle OC1A on Compare Match.
 *    1  |   0  | Clear OC1A on Compare Match when up-counting. Set OC1A on
 *       |      | Compare Match when down-counting.
 *    1  |   1  | Set OC1A on Compare Match when up-counting. Clear OC1A on
 *       |      |Compare Match when down-counting.
 *
 * The COM1A bits in TCCR1A need to be set using one of the defines
 * below.
 */
#define TC1_TCCR1A_OC1A_MODE0 0
#define TC1_TCCR1A_OC1A_MODE1 1
#define TC1_TCCR1A_OC1A_MODE2 2
#define TC1_TCCR1A_OC1A_MODE3 3

#define TC1_TCCR1A_OC1B_MODE0 0
#define TC1_TCCR1A_OC1B_MODE1 1
#define TC1_TCCR1A_OC1B_MODE2 2
#define TC1_TCCR1A_OC1B_MODE3 3

/* There are 8 clock sources for TC1. The clockSelect bits are set by writing
 * to TCCR1B using one of the defines below.
 */
#define TC1_TCCR1B_CLK_NONE      0 /* no source, clock is stopped */
#define TC1_TCCR1B_CLK_PRSC1     1 /* no prescale */
#define TC1_TCCR1B_CLK_PRSC8     2 /* prescale divide by 8 */
#define TC1_TCCR1B_CLK_PRSC64    3 /* prescale divide by 64 */
#define TC1_TCCR1B_CLK_PRSC256   4 /* prescale divide by 256 */
#define TC1_TCCR1B_CLK_PRSC1024  5 /* prescale divide by 1024 */
#define TC1_TCCR1B_CLK_EXTFALL   6 /* external source clock on falling edge */
#define TC1_TCCR1B_CLK_EXTRISE   7 /* external source clock on rising edge */

typedef union {
  struct  {
    uint8_t wgm1l : 2;// waveform generation
    uint8_t : 2;
    uint8_t com1b : 2;// compare match output B
    uint8_t com1a : 2;// compare match output A
  };
  uint8_t reg;    
} TC1_TCCR1A_type;

typedef union {
  struct {
    uint8_t cs1 : 3;// clock source select
    uint8_t wgm1h : 2;// upper bit of waveform generation
    uint8_t : 1;
    uint8_t ices1 : 1;// input capture noise canceler
    uint8_t icnc1 : 1;// input capture edge select
  };
  uint8_t reg;
} TC1_TCCR1B_type;

typedef union {
  struct {
    uint8_t : 6;
    uint8_t foc1a : 1;
    uint8_t foc1b : 1;
  };
  uint8_t reg;    
} TC1_TCCR1C_type;

typedef union {
  struct {
    uint8_t tcnt1l;
    uint8_t tcnt1h;
  };
  uint16_t tcnt1_reg;
} TC1_TCNT1_type;

typedef union {
  struct {
    uint8_t ocr1al;
    uint8_t ocr1ah;
  };
  uint16_t ocr1a_reg;
} TC1_OCR1A_type;

typedef union {
  struct {
    uint8_t ocr1bl;
    uint8_t ocr1bh;
  };
  uint16_t ocr1b_reg;
} TC1_OCR1B_type;

typedef union {
  struct {
    uint8_t icr1l;
    uint8_t icr1h;
  };
  uint16_t icr1_reg;
} TC1_ICR1_type;

typedef union {
  struct {
    uint8_t toie1 : 1;// timer overflow interrupt enable
    uint8_t ocie1a : 1;// output compare A interrupt enable
    uint8_t ocie1b : 1;// output compare B interrupt enable
    uint8_t : 2;
    uint8_t icie1 : 1;// input capture interrupt enable
    uint8_t : 2;
  };
  uint8_t reg;
} TC1_TIMSK1_type;

typedef union {
  struct {
    uint8_t tov1 : 1;// timer overflow interrupt flag
    uint8_t ocif1a : 1;// output compare A interrupt flag
    uint8_t ocif1b : 1;// output compare B interrupt flag
    uint8_t : 2;
    uint8_t icf1 : 1;// input capture interrupt enable
    uint8_t : 2;
  };
  uint8_t reg;
} TC1_TIFR1_type;

typedef struct {
  TC1_TCCR1A_type tccr1a;
  TC1_TCCR1B_type tccr1b;
  TC1_TCCR1C_type tccr1c;
  TC1_TCNT1_type  tcnt1;
  TC1_OCR1A_type  ocr1a;
  TC1_OCR1B_type  ocr1b;
  TC1_ICR1_type   icr1;
  TC1_TIMSK1_type timsk1;
  TC1_TIFR1_type  tifr1;
} Timer_Counter1;

void tc1_get_config(Timer_Counter1 *timer);
void tc1_set_config(Timer_Counter1 *timer);

/* Write the value to the OCR1A register.  */
void tc1_set_ocr1a(uint16_t val);

/* Write the value to the OCR1B register.
 */
void tc1_set_ocr1b(uint16_t val);

#endif /* _TC1_H_ */
