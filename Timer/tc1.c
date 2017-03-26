/*
 * tc1.c
 *
 * Created: 2016-08-25
 *  Author: Craig Hollinger
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 3
 * or the GNU Lesser General Public License version 3, both as
 * published by the Free Software Foundation.
*/ 
 
#include <avr/io.h>
#include <timer/tc1.h>

/* tc1_get_config()
 *
 * Fill a pre-defined memory structure with all the Timer/Counter 1 registers.
 * The calling function will modify the structure as required, then call
 * tc1_set_config() to write the data back to the Timer/Counter 1 registers.
 *
 * Note that the 16-bit register reads are not atomic, but this function assumes
 * that interrupts are disabled.
 */
void tc1_get_config(Timer_Counter1 *timer)
{
  timer->tccr1a.reg = TCCR1A;
  timer->tccr1b.reg = TCCR1B;
  timer->tcnt1.tcnt1_reg = TCNT1;
  timer->ocr1a.ocr1a_reg = OCR1A;
  timer->ocr1b.ocr1b_reg = OCR1B;
  timer->timsk1.reg = TIMSK1;
  timer->tifr1.reg = TIFR1;

}/* end tc1_get_config() */

/* tc1_set_config()
 *
 * Write the contents of the Timer_Counter1 structure back to the Timer/Counter
 * 1 registers.
 *
 * Note that the 16-bit register writes are not atomic, but this function
 * assumes that interrupts are disabled.
 */
void tc1_set_config(Timer_Counter1 *timer)
{
  TCCR1A = timer->tccr1a.reg;
  TCCR1B = timer->tccr1b.reg;
  TCNT1 = timer->tcnt1.tcnt1_reg;
  OCR1A = timer->ocr1a.ocr1a_reg;
  OCR1B = timer->ocr1b.ocr1b_reg;
  TIMSK1 = timer->timsk1.reg;
  TIFR1 = timer->tifr1.reg;

}/* end tc1_set_config() */

/* Write the value to the OCR1A register.
 */
void tc1_set_ocr1a(uint16_t val){
  OCR1A = val;
}

/* Write the value to the OCR0B register.
 */
void tc1_set_ocr1b(uint16_t val){
  OCR1B = val;
}
